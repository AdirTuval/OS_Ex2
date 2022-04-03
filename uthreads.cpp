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
int uthread_resume(int tid);
int uthread_sleep(int num_quantums);
int uthread_get_tid();
int uthread_get_total_quantums();
int uthread_get_quantums(int tid);

int main(){
    uthread_init(50);
    printf("Hello World");
	return 0;
}