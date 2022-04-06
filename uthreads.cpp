//
// Created by adirt on 01/04/2022.
//
#include <csignal>
#include <cassert>
#include "uthreads.h"
#include "scheduler.h"

#define FAILURE -1
#define SUCCESS 0
#define ERR_MSG_MAX_THREADS_EXCEEDED "thread library error: max threads exceeded.\n";
#define ERR_MSG_MAX_INVALID_QUANTUM "thread library error: non-positive quantum usecs.\n";
#define ERR_MSG_MAX_INVALID_ENTRY_POINT "thread library error: null entry point.\n";
#define ERR_MSG_THREAD_NOT_FOUND "thread library error: thread not found.\n";
#define ERR_MSG_MAIN_THREAD_BLOCK "thread library error: can not block main thread.\n";
#define ERR_MSG_MAIN_THREAD_SLEEP "thread library error: can not sleep main thread.\n";
int uthread_init(int quantum_usecs){
	if(quantum_usecs <= 0){
        cerr << ERR_MSG_MAX_INVALID_QUANTUM;
        return FAILURE;
    }
	Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    Scheduler::getInstance(quantum_usecs);
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
	return SUCCESS;
}
int uthread_spawn(thread_entry_point entry_point){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    if(entry_point == nullptr){
        cerr << ERR_MSG_MAX_INVALID_ENTRY_POINT;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
	Scheduler &scheduler = Scheduler::getInstance();
	int new_thread_id = scheduler.create_thread_and_push_to_ready(entry_point);
    if(new_thread_id == FAILURE){
        cerr << ERR_MSG_MAX_THREADS_EXCEEDED;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
	return new_thread_id;
}
int uthread_terminate(int tid){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
	Scheduler &scheduler = Scheduler::getInstance();
	if(tid == MAIN_THREAD_ID){
		exit(0);
	}
	int terminated_thread_id = scheduler.terminate_thread(tid);
	if(terminated_thread_id == FAILURE){
	    cerr << ERR_MSG_THREAD_NOT_FOUND;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
	return terminated_thread_id;
}
int uthread_block(int tid){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    if(tid == MAIN_THREAD_ID){
        cerr << ERR_MSG_MAIN_THREAD_BLOCK;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    Scheduler &scheduler = Scheduler::getInstance();
    int blocked_thread_id = scheduler.block_thread(tid);
    if(blocked_thread_id == FAILURE){
        cerr << ERR_MSG_THREAD_NOT_FOUND;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return blocked_thread_id;
}
int uthread_resume(int tid){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    Scheduler &scheduler = Scheduler::getInstance();
    int resumed_thread_id = scheduler.resume_thread(tid);
    if(resumed_thread_id == FAILURE){
        cerr << ERR_MSG_THREAD_NOT_FOUND;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return resumed_thread_id;
}
int uthread_sleep(int num_quantums){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    Scheduler &scheduler = Scheduler::getInstance();
    if(scheduler.get_current_running_thread_id() == MAIN_THREAD_ID){
        cerr << ERR_MSG_MAIN_THREAD_SLEEP;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    int sleep_thread_id = scheduler.sleep_thread(num_quantums);
    if(sleep_thread_id == FAILURE){
        cerr << ERR_MSG_THREAD_NOT_FOUND;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return sleep_thread_id;
}
int uthread_get_tid(){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    int ret_val = Scheduler::getInstance().get_current_running_thread_id();
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return ret_val;
}
int uthread_get_total_quantums(){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    int ret_val = Scheduler::getInstance().get_total_quantoms();
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return ret_val;

}
int uthread_get_quantums(int tid){
    Scheduler::change_blockage_status_sigvtalrm(SIG_BLOCK);
    int quantums_num = Scheduler::getInstance().get_quantoms_running_num(tid);
    if(quantums_num == FAILURE){
        cerr << ERR_MSG_THREAD_NOT_FOUND;
        Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
        return FAILURE;
    }
    Scheduler::change_blockage_status_sigvtalrm(SIG_UNBLOCK);
    return quantums_num;
}