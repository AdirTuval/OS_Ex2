//
// Created by adirt on 03/04/2022.
//

#include "thread.h"
#include "utils.h"
#define MAIN_THREAD_ID 0
Thread::Thread(int id ,thread_entry_point entry_point) : _id(id), _state(ThreadState::READY), _env{0}, _is_sleeping(false){
    //TODO if id is unused remove it
    _stack = new char[STACK_SIZE];
//    int ret_val = sigsetjmp(_env, 1); // TODO check if savemask == 1
//    if(ret_val != 0 && id != MAIN_THREAD_ID){
//        address_t pc = (address_t) entry_point;
//        (_env->__jmpbuf)[JB_PC] = translate_address(pc);
//        address_t sp = (address_t) _stack + STACK_SIZE - sizeof(address_t);
//        (_env->__jmpbuf)[JB_SP] = translate_address(sp);
//    }
//    sigemptyset(&_env->__saved_mask);
}

void Thread::do_jump(){
    siglongjmp(_env, _id);
}

void Thread::set_jump() {

}
