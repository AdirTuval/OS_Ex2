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
//        auto * new_thread = new Thread(thread_id, entry_point);
        auto * new_thread = new Thread(thread_id);
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
    if(thread_to_terminate->get_state() == ThreadState::RUNNING){
        //run new thread
        //		_run_top_thread();
    }
    else if(thread_to_terminate->get_state() == ThreadState::READY){
        if(_erase_from_ready_queue(tid) == FAILURE){
            //TODO print error
            return FAILURE;
        }
    }
    _active_threads.erase(tid);
    _unused_threads_id.push(tid);
    delete thread_to_terminate;
    return SUCCESS;
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