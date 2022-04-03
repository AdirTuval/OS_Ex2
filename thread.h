//
// Created by adirt on 03/04/2022.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include <csetjmp>
#include <csignal>
#include <cstdio>
#include "uthreads.h"
enum ThreadState {RUNNING, READY, BLOCKED, TERMINATED};
class Thread{
private:
    ThreadState _state;
    sigjmp_buf _env;
    char * _stack;
    const int _id;
public:
    explicit Thread(int id ,thread_entry_point entry_point = nullptr);
    int get_id() const {return _id;}
    ThreadState get_state() const {return _state;}
    void set_state(ThreadState new_state) {_state = new_state;}
    ~Thread(){
        delete[] _stack;
        printf("Thread deleted");
    }

};


#endif //EX2_THREAD_H
