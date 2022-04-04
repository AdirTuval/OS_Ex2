//
// Created by adirt on 01/04/2022.
//
#include <cstdio>
#include "uthreads.h"
#include "scheduler.h"
#define FAILURE -1
#define SUCCESS 0

int uthread_init(int quantum_usecs){
	if(quantum_usecs <= 0){
		return FAILURE;
	}
	Scheduler::getInstance(quantum_usecs);
	return SUCCESS;
}
int uthread_spawn(thread_entry_point entry_point){
	Scheduler &scheduler = Scheduler::getInstance();
	return scheduler.create_thread(entry_point);
}
int uthread_terminate(int tid){
	Scheduler &scheduler = Scheduler::getInstance();
	if(tid == 0){
		scheduler.~Scheduler();
		exit(0);
	}
	return scheduler.terminate_thread(tid);
}
int uthread_block(int tid){
    if(tid == 0){
        return FAILURE;
    }
    Scheduler &scheduler = Scheduler::getInstance();
    return scheduler.block_thread(tid);
}
int uthread_resume(int tid){
    Scheduler &scheduler = Scheduler::getInstance();
    return scheduler.resume_thread(tid);
}
int uthread_sleep(int num_quantums){
    Scheduler &scheduler = Scheduler::getInstance();
    if(scheduler.get_current_running_thread_id() == MAIN_THREAD_ID){
        return FAILURE;
    }
    scheduler.sleep_thread(num_quantums);
    return 0;
}
int uthread_get_tid(){
    return Scheduler::getInstance().get_current_running_thread_id();
}
int uthread_get_total_quantums(){
    return Scheduler::getInstance().get_total_quantoms();
}
int uthread_get_quantums(int tid){
    return Scheduler::getInstance().get_quantoms_running_num(tid);
}

int main(){
    uthread_init(1000000L);
    uthread_spawn(nullptr);
    uthread_spawn(nullptr);
    Scheduler &scheduler = Scheduler::getInstance();
    int quantoms = 4;
    int cur_id = 0;
    bool new_id = true;
    for(;;){

    }
	return 0;
}