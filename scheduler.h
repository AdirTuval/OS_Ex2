//
// Created by adirt on 03/04/2022.
//

#ifndef EX2_SCHEDULER_H
#define EX2_SCHEDULER_H
#define FIRST_ID 0
#include <bits/stdc++.h>
#include <csetjmp>
#include "thread.h"

using namespace std;

class Scheduler{
private:
    explicit Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs), _active_thread_id(FIRST_ID){
        _unused_threads_id.push(FIRST_ID);
        _largest_thread_id = FIRST_ID;
        create_thread(nullptr);
    }
    const int _quantum_usecs;
    int _largest_thread_id;
    int _active_thread_id;
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

    ~Scheduler(){
        if(!_active_threads.empty()) {
            for(unordered_map<int,Thread *>::iterator it=_active_threads.begin() ; it!=_active_threads.end() ; it++){
                delete it->second;
            }
            _active_threads.clear();
        }
    }
};


#endif //EX2_SCHEDULER_H
