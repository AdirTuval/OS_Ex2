//
// Created by adirt on 03/04/2022.
//

#include "scheduler.h"
#define FAILURE -1
#define SUCCESS 0
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
    if(thread_to_terminate->get_state() == ThreadState::RUNNING || thread_to_terminate->get_state() == ThreadState::BLOCKED){
        _active_threads.erase(tid);
        _unused_threads_id.push(tid);
        delete thread_to_terminate;
         _run_top_thread();
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
    for (deque<Thread *>::iterator it = _ready_queue.begin(); it != _ready_queue.end(); ++it){
        if((*it)->get_id() == tid){
            _ready_queue.erase(it);
            return SUCCESS;
        }
    }
    return FAILURE;
}

int Scheduler::_run_top_thread(){
    //pop from queue
    //change running current thread id
    //jump
    if(_ready_queue.empty()){
        return FAILURE;
    }
    Thread * front_thread = _ready_queue.front();
    _ready_queue.pop_front();
    assert(front_thread->get_state() == ThreadState::READY);
    front_thread->set_running();
    _running_thread_id = front_thread->get_id();
    front_thread->jump();
    return SUCCESS;
}

int Scheduler::block_thread(int tid) {
    //Assuming tid != 0
    assert(tid != 0);
    if(_active_threads.find(tid) == _active_threads.end()){
        return FAILURE;
    }
    Thread * thread_to_block = _active_threads[tid];
    thread_to_block->set_block();
    switch(thread_to_block->get_state()){
        case RUNNING:
            assert(_running_thread_id == tid);
            _run_top_thread();

            break;
        case READY:
            if(_erase_from_ready_queue(tid) != SUCCESS){
                return FAILURE;
            }
            break;
        case BLOCKED:
            return SUCCESS;
    }
    return SUCCESS;
}