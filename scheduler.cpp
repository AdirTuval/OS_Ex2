//
// Created by adirt on 03/04/2022.
//

#include "scheduler.h"
#define FAILURE -1
#define SUCCESS 0
#define USEC_IN_SEC 1000000
#define ERR_MSG_BAD_ALLOCATION "system error: stack allocation failed.\n"

Scheduler::Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs), _running_thread_id(MAIN_THREAD_ID), _total_quantoms(1), _largest_thread_id(MAIN_THREAD_ID){
    _unused_threads_id.push(MAIN_THREAD_ID);
    Thread * main_thread = create_thread(nullptr);
    assert(main_thread != nullptr);
    _active_threads[MAIN_THREAD_ID] = main_thread;
    main_thread->set_running();
    init_timer_for_a_quantom();
}

int Scheduler::generate_thread_id()
{
    if(!_unused_threads_id.empty()){
        //There's an available used id.
        int id = _unused_threads_id.top();
        _unused_threads_id.pop();
        return id;
    }
    //No used ID, generate new ID.
    return ++_largest_thread_id;
}

int Scheduler::init_timer_for_a_quantom() const{
    struct sigaction sa = {nullptr};
    sa.sa_handler = &static_external_time_handler;
    if (sigaction(SIGVTALRM, &sa, nullptr) < 0) {
        printf("sigaction error.");
        return FAILURE;
    }
    struct itimerval timer = {0};
    timer.it_value.tv_sec = _quantum_usecs / USEC_IN_SEC;        // first time interval, seconds part
    timer.it_value.tv_usec = _quantum_usecs % USEC_IN_SEC;        // first time interval, microseconds part

    // configure the timer to expire every 3 sec after that.
    timer.it_interval.tv_sec = 0;    // following time intervals, seconds part
    timer.it_interval.tv_usec = 0;    // following time intervals, microseconds part

    if (setitimer(ITIMER_VIRTUAL, &timer, nullptr)){
        printf( "%s", std::strerror(errno) );
        printf("setitimer error.\n");
        return FAILURE;
    }
    return SUCCESS;
}

int Scheduler::create_thread_and_push_to_ready(thread_entry_point entry_point)
{
    Thread * new_thread = create_thread(entry_point);
    if(new_thread == nullptr){
        return FAILURE;
    }
    _active_threads[new_thread->get_id()] = new_thread;
    assert(new_thread->is_ready());
    _ready_queue.push_back(new_thread);
    return new_thread->get_id();
}

int Scheduler::terminate_thread(int tid) {
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    Thread * thread_to_terminate = _active_threads[tid];
    _active_threads.erase(tid);
    _unused_threads_id.push(tid);

    if(thread_to_terminate->is_running()){
        delete thread_to_terminate;
        run_topmost_thread_in_ready_queue();
        return SUCCESS;
    }
    if(thread_to_terminate->is_sleeping()){
        _sleeping_threads.erase(thread_to_terminate);
    }
    if(thread_to_terminate->is_ready()){
        erase_thread_from_ready_queue(tid);
    }
    delete thread_to_terminate;
    return SUCCESS;
}

int Scheduler::erase_thread_from_ready_queue(int tid) {
    for (auto it = _ready_queue.begin(); it != _ready_queue.end(); ++it){
        if((*it)->get_id() == tid){
            _ready_queue.erase(it);
            return SUCCESS;
        }
    }
    return FAILURE;
}

Scheduler &Scheduler::getInstance(int quantum_usecs) {
    static Scheduler instance(quantum_usecs);
    return instance;
}

int Scheduler::run_topmost_thread_in_ready_queue(){
    //pop from queue
    //change running current thread id
    //init timer for a quantom
    _total_quantoms++;
    update_sleeping_threads();
    if(_ready_queue.empty()){
        return FAILURE;
    }
    Thread * front_thread = _ready_queue.front();
    _ready_queue.pop_front();
    assert(front_thread->is_ready());
    front_thread->set_running();
    _running_thread_id = front_thread->get_id();
    init_timer_for_a_quantom();
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    front_thread->do_jump();
    return SUCCESS;
}

int Scheduler::block_thread(int tid) {
    assert(tid != 0);
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    Thread * thread_to_block = _active_threads[tid];
    if(thread_to_block->is_running()){
        assert(_running_thread_id == tid);
        bool executed_from_jump = thread_to_block->save_env();
        if(executed_from_jump){
            return SUCCESS;
        }
        thread_to_block->set_block();
        run_topmost_thread_in_ready_queue();
        //TODO this is delicate place to check.
    }else if(thread_to_block->is_ready()){
        if(erase_thread_from_ready_queue(tid) != SUCCESS){
            return FAILURE;
        }
        thread_to_block->set_block();
    }
    return SUCCESS;
}

int Scheduler::resume_thread(int tid) {
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    Thread * thread_to_resume = _active_threads[tid];
    if(not thread_to_resume->is_blocked()){
        return SUCCESS;
    }
    thread_to_resume->unblock();
    if(thread_to_resume->is_ready()){
        _ready_queue.push_back(thread_to_resume);
    }
    return SUCCESS;
}

int Scheduler::sleep_thread(int num_quantums) {
    assert(_running_thread_id != MAIN_THREAD_ID);
    Thread * running_thread = _active_threads[_running_thread_id];
    running_thread->set_sleep(num_quantums);
    _sleeping_threads.insert(running_thread);
    bool exectued_from_jump = running_thread->save_env();
    if(exectued_from_jump){
        return SUCCESS;
    }
    run_topmost_thread_in_ready_queue();
    return SUCCESS;
}

void Scheduler::internal_time_handler() {
    Thread * running_thread = _active_threads[_running_thread_id];
    running_thread->set_ready();
    _ready_queue.push_back(running_thread);
    bool executed_from_jump = running_thread->save_env();
    if(executed_from_jump){
        return;
    }
    run_topmost_thread_in_ready_queue();
}

Scheduler::~Scheduler() {
    for (auto it = _active_threads.cbegin(); it != _active_threads.cend();){
        delete it->second;
        _active_threads.erase(it++);    // or "it = m.erase(it)" since C++11
    }
}

void Scheduler::update_sleeping_threads() {
    for(auto it = _sleeping_threads.begin(); it != _sleeping_threads.end(); ) {
        auto &cur_thread = **it;
        cur_thread.update_sleep_value();
        if(!cur_thread.is_sleeping()){
            it = _sleeping_threads.erase(it);
            if(cur_thread.is_ready()){
                _ready_queue.push_back(_active_threads[cur_thread.get_id()]);
            }
        }
        else{
            ++it;
        }
    }
}

int Scheduler::get_total_quantoms() const {
    return _total_quantoms;
}

int Scheduler::get_quantoms_running_num(int tid) {
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    return _active_threads[tid]->get_age();
}

void Scheduler::static_external_time_handler(int sig) {
    Scheduler &scheduler = getInstance();
    scheduler.internal_time_handler();
}

Thread *Scheduler::create_thread(thread_entry_point entry_point) {
    if(_active_threads.size() >= MAX_THREAD_NUM){
        return nullptr;
    }
    int thread_id = generate_thread_id();
    Thread * new_thread;
    try {
        new_thread = new Thread(thread_id, entry_point);
    } catch (bad_alloc&){
        cerr << ERR_MSG_BAD_ALLOCATION;
        exit(1);
    }
    return new_thread;
}

void Scheduler::change_blockage_status_sigvtalrm(int operation) {
    assert(operation == SIG_UNBLOCK or operation == SIG_BLOCK);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(operation, &set, nullptr);
}

