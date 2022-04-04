//
// Created by adirt on 03/04/2022.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include <csetjmp>
#include <csignal>
#include <cstdio>
#include "uthreads.h"
enum ThreadState {RUNNING, READY, BLOCKED};
class Thread{
private:
    ThreadState _state;
    sigjmp_buf _env;
    char * _stack;
    const int _id;
    bool _is_sleeping;
public:
    explicit Thread(int id ,thread_entry_point entry_point = nullptr);
    int get_id() const {return _id;}
    ThreadState get_state() const {return _state;}
    void set_block() {_state = ThreadState::BLOCKED;}
    void set_running() {_state = ThreadState::RUNNING;}
    void set_ready() {_state = ThreadState::READY;}
    void go_to_sleep() { _is_sleeping = true;}
    bool is_sleeping() const {return _is_sleeping;}
    void do_jump();
    ~Thread(){
        delete[] _stack;
        printf("Thread deleted");
    }

    bool is_running() const {return _state == ThreadState::RUNNING;}
    bool is_blocked() const {return _state == ThreadState::BLOCKED;}
    bool is_ready() const {return _state == ThreadState::READY;}

    void set_jump();
};


#endif //EX2_THREAD_H
