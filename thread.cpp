//
// Created by adirt on 03/04/2022.
//

#include "thread.h"
#include "utils.h"
#include <cassert>
#include <new>
#include <iostream>
#define MAIN_THREAD_ID 0
#define NOT_SLEEPING  -1

Thread::Thread(int id, thread_entry_point entry_point) :
        _id(id), _state(ThreadState::READY), _env{0}, _quantoms_left_to_sleep(NOT_SLEEPING), _running_quantoms_num(0){
    if(id != MAIN_THREAD_ID){
        setup_env(entry_point);
    }
}

void Thread::do_jump(){
    siglongjmp(_env, 3);
}

bool Thread::is_blocked(bool check_if_block_only) const {
    if (_state == ThreadState::BLOCKED){
        return true;
    }
    return !check_if_block_only && _state == ThreadState::BLOCKED_AND_SLEEPING;
}

bool Thread::is_sleeping(bool check_if_sleeping_only) const {
    if (_state == ThreadState::SLEEPING){
        return true;
    }
    return !check_if_sleeping_only && _state == ThreadState::BLOCKED_AND_SLEEPING;
}

void Thread::set_sleep(int num_quantoms) {
    _state = _state == ThreadState::BLOCKED ? ThreadState::BLOCKED_AND_SLEEPING : ThreadState::SLEEPING;
    _quantoms_left_to_sleep = num_quantoms;
}

void Thread::update_sleep_value() {
    assert(is_sleeping() && _quantoms_left_to_sleep != NOT_SLEEPING);
    if(_quantoms_left_to_sleep >= 1){
        --_quantoms_left_to_sleep;
    }else{
        _quantoms_left_to_sleep = NOT_SLEEPING;
        wake_up();
    }
}

void Thread::wake_up() {
    assert(is_sleeping());
    printf("Thread %d is waking up.\n", _id);
    if(_state == ThreadState::SLEEPING){
        _state = ThreadState::READY;
    }else if(_state == ThreadState::BLOCKED_AND_SLEEPING){
        _state = ThreadState::BLOCKED;
    }
}

void Thread::set_running() {
    _state = ThreadState::RUNNING;
    _running_quantoms_num++;
}

void Thread::unblock() {
    if(!is_blocked()){
        return;
    }
    if(_state == ThreadState::BLOCKED){
        _state = ThreadState::READY;
    }else{ //_state == ThreadState::BLOCKED_AND_SLEEPING
        _state = ThreadState::SLEEPING;
    }
}

int Thread::save_env() {
    bool executed_from_jump = sigsetjmp(_env, 1) != 0;
    return executed_from_jump;
}

void Thread::setup_env(thread_entry_point entry_point) {
    int ret_val = sigsetjmp(_env, 1);
    assert(ret_val == 0);
    bool did_just_save_bookmark = ret_val == 0;
    if(did_just_save_bookmark){
        if(entry_point != nullptr){
            address_t pc = (address_t) entry_point;
            (_env->__jmpbuf)[JB_PC] = translate_address(pc);
            address_t sp = (address_t) _stack + STACK_SIZE - sizeof(address_t);
            (_env->__jmpbuf)[JB_SP] = translate_address(sp);
        }
        sigemptyset(&(_env->__saved_mask));
    }

}
