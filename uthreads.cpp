//
// Created by adirt on 01/04/2022.
//
#include <cstdio>
#include "uthreads.h"

/**
 * @brief initializes the thread library.
 *
 * You may assume that this function is called before any other thread library function, and that it is called
 * exactly once.
 * The input to the function is the length of a quantum in micro-seconds.
 * It is an error to call this function with non-positive quantum_usecs.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs);
int uthread_spawn(thread_entry_point entry_point);
int uthread_terminate(int tid);
int uthread_block(int tid);
int uthread_resume(int tid);
int uthread_sleep(int num_quantums);
int uthread_get_tid();
int uthread_get_total_quantums();
int uthread_get_quantums(int tid);

class Scheduler{
private:
	int _quantum_usecs;
public:
	Scheduler(int quantum_usecs) {
		_quantum_usecs = quantum_usecs;
	}
};



int main(){
	printf("hello world!");
	return 0;
}