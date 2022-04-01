//
// Created by adirt on 01/04/2022.
//
#include <cstdio>
#include <cassert>
#include "uthreads.h"
#define NOT_SET -1
class Scheduler{
private:
	explicit Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs){}
	Scheduler(Scheduler const&);              // Don't Implement
	void operator=(Scheduler const&);
	const int _quantum_usecs;
public:
	static Scheduler& getInstance(int quantum_usecs = 0){
		static Scheduler instance(quantum_usecs);
		return instance;
	}
	int getQuantumUsecs() const {return _quantum_usecs;}
};

/**
 * Global scheduler.
 */
int uthread_init(int quantum_usecs){

}
int uthread_spawn(thread_entry_point entry_point);
int uthread_terminate(int tid);
int uthread_block(int tid);
int uthread_resume(int tid);
int uthread_sleep(int num_quantums);
int uthread_get_tid();
int uthread_get_total_quantums();
int uthread_get_quantums(int tid);


int main(){
	Scheduler &t = Scheduler::getInstance(20);
	printf("%d", t.getQuantumUsecs());
	Scheduler &c = Scheduler::getInstance(500);
	printf("%d", c.getQuantumUsecs());
	return 0;
}