//
// Created by adirt on 03/04/2022.
//

#ifndef EX2_SCHEDULER_H
#define EX2_SCHEDULER_H
#define MAIN_THREAD_ID 0
#include <bits/stdc++.h>
#include <csetjmp>
#include "thread.h"

using namespace std;

class Scheduler{
private:
    explicit Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs), _running_thread_id(MAIN_THREAD_ID){
        _unused_threads_id.push(MAIN_THREAD_ID);
        _largest_thread_id = MAIN_THREAD_ID;
        create_thread(nullptr);
        _ready_queue.pop_front(); //popping first thread.
        _active_threads[MAIN_THREAD_ID]->set_running();
    }
    const int _quantum_usecs;
    int _largest_thread_id;
    int _running_thread_id;
    priority_queue <int, vector<int>, greater<int>> _unused_threads_id;
    unordered_map<int, Thread *> _active_threads;
    deque<Thread *> _ready_queue;
public:
    int create_thread(thread_entry_point entry_point);
    int terminate_thread(int tid);
    int _generate_thread_id();
    Scheduler(Scheduler const&) = delete;
    void operator=(Scheduler const&) = delete;
    int _erase_from_ready_queue(int tid);
    static Scheduler& getInstance(int quantum_usecs = 0){
        static Scheduler instance(quantum_usecs);
        return instance;
    }
    int _run_top_thread();
    int block_thread(int tid);

    ~Scheduler(){
        if(!_active_threads.empty()) {
            for(unordered_map<int,Thread *>::iterator it=_active_threads.begin() ; it!=_active_threads.end() ; it++){
                delete it->second;
            }
            _active_threads.clear();
        }
        printf("Scheduler deleted.");
    }
};


#endif //EX2_SCHEDULER_H
