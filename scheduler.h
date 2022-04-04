//
// Created by adirt on 03/04/2022.
//

#ifndef EX2_SCHEDULER_H
#define EX2_SCHEDULER_H
#define MAIN_THREAD_ID 0
#include <bits/stdc++.h>
#include <csetjmp>
#include <cstdio>
#include <csignal>
#include <sys/time.h>
#include <ctime>
#include "thread.h"
using namespace std;
void time_handler(int sig);
class Scheduler{
private:
    explicit Scheduler(int quantum_usecs);
    const int _quantum_usecs;
    int _largest_thread_id;
    int _running_thread_id;
    int _total_quantoms;
    priority_queue <int, vector<int>, greater<int>> _unused_threads_id;
    unordered_map<int, Thread *> _active_threads;
    unordered_set<Thread *> _sleeping_threads; //_sleeping_threads
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
    int run_topmost_thread_in_queue();
    int block_thread(int tid);
    int resume_thread(int tid);
    int sleep_thread(int num_quantums);
    int _init_timer_for_a_quantom() const;
    int get_current_running_thread_id() const {return _running_thread_id;}
    void _time_handler();
    Thread *stop_and_retrieve_running_thread();
    void update_sleeping_threads();
    int get_quantoms_running_num(int tid);
    ~Scheduler();

    int get_total_quantoms() const;
};


#endif //EX2_SCHEDULER_H
