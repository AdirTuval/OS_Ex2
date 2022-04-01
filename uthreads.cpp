//
// Created by adirt on 01/04/2022.
//
#include <cstdio>
#include <cassert>
#include "uthreads.h"
#include <bits/stdc++.h>
#define FIRST_ID 0
using namespace std;

class Scheduler{
private:
	explicit Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs){
		_threads_id_queue.push(FIRST_ID);
	}
	const int _quantum_usecs;
	priority_queue <int, vector<int>, greater<int>> _threads_id_queue;
public:
	Scheduler(Scheduler const&) = delete;
	void operator=(Scheduler const&) = delete;
	static Scheduler& getInstance(int quantum_usecs = 0){
		static Scheduler instance(quantum_usecs);
		return instance;
	}
};

int uthread_init(int quantum_usecs){
	Scheduler::getInstance(quantum_usecs);
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
	printf("hello");
	return 0;
}