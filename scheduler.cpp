//
// Created by adirt on 03/04/2022.
//

#include "scheduler.h"
#define FAILURE -1
#define SUCCESS 0
#define USEC_IN_SEC 1000000

int t = 0;
int blocked_thread_id = -1;
void time_handler(int sig){
    t++;
    Scheduler &scheduler = Scheduler::getInstance();
    scheduler._time_handler();
    printf("Current running thread: %d. Quantoms alive: %d. Total quantoms: %d.\n", uthread_get_tid(), uthread_get_quantums(uthread_get_tid()), uthread_get_total_quantums());
    if(t == 5){
        printf("Thread %d is blocked.\n",uthread_get_tid());
        blocked_thread_id = uthread_get_tid();
        uthread_block(blocked_thread_id);
    }
    if(t == 10){
        printf("Thread %d is resumed..\n",blocked_thread_id);
        uthread_resume(blocked_thread_id);
    }
}

Scheduler::Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs), _running_thread_id(MAIN_THREAD_ID), _total_quantoms(1){
    _unused_threads_id.push(MAIN_THREAD_ID);
    _largest_thread_id = MAIN_THREAD_ID;
    create_thread(nullptr);
    run_topmost_thread_in_queue();
}

int Scheduler::_generate_thread_id()
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

int Scheduler::_init_timer_for_a_quantom() const{
    struct sigaction sa = {nullptr};
    sa.sa_handler = &time_handler;
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

int Scheduler::create_thread(thread_entry_point entry_point)
{
    if(_active_threads.size() >= MAX_THREAD_NUM){
        return FAILURE;
    }
    try {
        int thread_id = _generate_thread_id();
        auto * new_thread = new Thread(thread_id, entry_point);
        _active_threads[thread_id] = new_thread;
        _ready_queue.push_back(new_thread);
        return thread_id;
    } catch (bad_alloc&){
        return FAILURE;
    }
}

int Scheduler::terminate_thread(int tid) {
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    //TODO if thread is running, then we should push new thread to running
    Thread * thread_to_terminate = _active_threads[tid];
//    if(thread_to_terminate->get_state() == ThreadState::RUNNING || thread_to_terminate->get_state() == ThreadState::BLOCKED){
    if(thread_to_terminate->is_running() || thread_to_terminate->is_blocked()){
        _active_threads.erase(tid);
        _unused_threads_id.push(tid);
        delete thread_to_terminate;
        run_topmost_thread_in_queue();
         return SUCCESS;
    }
    else { // Thread is ready.
        if(_erase_from_ready_queue(tid) == FAILURE){
            //TODO print error
            return FAILURE;
        }
        return SUCCESS;
    }
}

int Scheduler::_erase_from_ready_queue(int tid) {
    for (auto it = _ready_queue.begin(); it != _ready_queue.end(); ++it){
        if((*it)->get_id() == tid){
            _ready_queue.erase(it);
            return SUCCESS;
        }
    }
    return FAILURE;
}

int Scheduler::run_topmost_thread_in_queue(){
    //pop from queue
    //change running current thread id
    //init timer for a quantom
    //do_jump
    if(_ready_queue.empty()){
        return FAILURE;
    }
    Thread * front_thread = _ready_queue.front();
    _ready_queue.pop_front();
    assert(front_thread->is_ready());
    front_thread->set_running();
    _running_thread_id = front_thread->get_id();
    _init_timer_for_a_quantom();
//    front_thread->do_jump();
    return SUCCESS;
}

int Scheduler::block_thread(int tid) {
    //Assuming tid != 0
    assert(tid != 0);
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    Thread * thread_to_block = _active_threads[tid];
    if(thread_to_block->is_running()){
        assert(_running_thread_id == tid);
        stop_and_retrieve_running_thread();
        run_topmost_thread_in_queue();
    }else if(thread_to_block->is_ready()){
        if(_erase_from_ready_queue(tid) != SUCCESS){
            return FAILURE;
        }
    }
    thread_to_block->set_block();
    return SUCCESS;
}

int Scheduler::resume_thread(int tid) {
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    Thread * thread_to_resume = _active_threads[tid];
    thread_to_resume->unblock();
    if(thread_to_resume->is_ready()){
        _ready_queue.push_back(thread_to_resume);
    }
    return SUCCESS;
}

int Scheduler::sleep_thread(int num_quantums) {
    assert(_running_thread_id != MAIN_THREAD_ID);
    Thread * running_thread = stop_and_retrieve_running_thread();
    running_thread->set_sleep(num_quantums);
    _sleeping_threads.insert(running_thread);
    run_topmost_thread_in_queue();
    return SUCCESS;
}

void Scheduler::_time_handler() {
    _total_quantoms++;
    update_sleeping_threads();
    Thread * running_thread = stop_and_retrieve_running_thread();
    running_thread->set_ready();
    _ready_queue.push_back(running_thread);
    run_topmost_thread_in_queue();
}

Thread * Scheduler::stop_and_retrieve_running_thread() {
    Thread * running_thread = _active_threads[_running_thread_id];
//    running_thread->set_jump();
    assert(running_thread->is_running());
    return running_thread;
}

Scheduler::~Scheduler() {
    if(!_active_threads.empty()) {
        for(auto & _active_thread : _active_threads){
            delete _active_thread.second;
        }
        _active_threads.clear();
    }
    printf("Scheduler deleted.");
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

