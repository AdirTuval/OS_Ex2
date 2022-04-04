//
// Created by adirt on 03/04/2022.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include <csetjmp>
#include <csignal>
#include <cstdio>
#include "uthreads.h"
class Thread{
private:
    enum ThreadState {RUNNING, READY, BLOCKED, SLEEPING, BLOCKED_AND_SLEEPING};
    ThreadState _state;
    sigjmp_buf _env;
    char * _stack;
    const int _id;
    int _quantoms_left_to_sleep;
    int _running_quantoms_num;
public:
    explicit Thread(int id ,thread_entry_point entry_point = nullptr);
    int get_id() const {return _id;}
    ThreadState get_state() const {return _state;}
    void set_block() {_state = _state == ThreadState::SLEEPING ? ThreadState::BLOCKED_AND_SLEEPING : ThreadState::BLOCKED;}
    void set_running();
    void set_ready() {_state = ThreadState::READY;}
    void set_sleep(int num_quantoms);
    void update_sleep_value();
    void wake_up();
    void do_jump();
    void set_jump();
    bool is_running() const {return _state == ThreadState::RUNNING;}
    bool is_blocked(bool check_if_block_only = false) const;
    bool is_sleeping(bool check_if_sleeping_only = false) const;
    bool is_ready() const {return _state == ThreadState::READY;}
    void grow_up() {++_running_quantoms_num;}
    int get_age() const {return _running_quantoms_num;}
    void unblock();
    ~Thread();
};


#endif //EX2_THREAD_H
