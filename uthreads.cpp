//
// Created by adirt on 01/04/2022.
//
#include "uthreads.h"
#include "scheduler.h"
#define FAILURE -1
#define SUCCESS 0
#define ERR_MSG_MAX_THREADS_EXCEEDED "thread library error: max threads exceeded.\n";
#define ERR_MSG_MAX_INVALID_QUANTUM "thread library error: non-positive quantum usecs.\n";

void change_blockage_status_sigvtalrm(int operation){
    assert(operation == SIG_UNBLOCK or operation == SIG_BLOCK);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(operation, &set, NULL);
}

int uthread_init(int quantum_usecs){
    change_blockage_status_sigvtalrm(SIG_BLOCK);
	if(quantum_usecs <= 0){
	    cerr << ERR_MSG_MAX_INVALID_QUANTUM;
		return FAILURE;
	}
	Scheduler::getInstance(quantum_usecs);
	change_blockage_status_sigvtalrm(SIG_UNBLOCK);
	return SUCCESS;
}
int uthread_spawn(thread_entry_point entry_point){
    change_blockage_status_sigvtalrm(SIG_BLOCK);
	Scheduler &scheduler = Scheduler::getInstance();
	int new_thread_id = scheduler.create_thread(entry_point);
    if(new_thread_id == FAILURE){
        cerr << ERR_MSG_MAX_THREADS_EXCEEDED;
        return FAILURE;
    }
    change_blockage_status_sigvtalrm(SIG_UNBLOCK);
	return new_thread_id;
}
int uthread_terminate(int tid){
    change_blockage_status_sigvtalrm(SIG_BLOCK);
	Scheduler &scheduler = Scheduler::getInstance();
	if(tid == MAIN_THREAD_ID){
		scheduler.~Scheduler();
		exit(0);
	}
	int ret_val = scheduler.terminate_thread(tid);
	change_blockage_status_sigvtalrm(SIG_UNBLOCK);
	return ret_val;
}
int uthread_block(int tid){
    change_blockage_status_sigvtalrm(SIG_BLOCK);
    if(tid == 0){
        return FAILURE;
    }
    Scheduler &scheduler = Scheduler::getInstance();
    int ret_val = scheduler.block_thread(tid);
    change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return ret_val;
}
int uthread_resume(int tid){
    change_blockage_status_sigvtalrm(SIG_BLOCK);
    Scheduler &scheduler = Scheduler::getInstance();
    int ret_val = scheduler.resume_thread(tid);
    change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return ret_val;
}
int uthread_sleep(int num_quantums){
    change_blockage_status_sigvtalrm(SIG_BLOCK);
    Scheduler &scheduler = Scheduler::getInstance();
    if(scheduler.get_current_running_thread_id() == MAIN_THREAD_ID){
        return FAILURE;
    }
    int ret_val = scheduler.sleep_thread(num_quantums);
    change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return ret_val;
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
    uthread_spawn(nullptr);
    uthread_spawn(nullptr);
    Scheduler &scheduler = Scheduler::getInstance();
    for(;;){}
	return 0;
}