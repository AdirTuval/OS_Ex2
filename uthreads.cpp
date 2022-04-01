//
// Created by adirt on 01/04/2022.
//
#include <cstdio>
#include <cassert>
#include "uthreads.h"
#include <bits/stdc++.h>
#define FIRST_ID 0
#define SUCCESS 0
#define FAILURE 1
using namespace std;
class Thread{

};

class Scheduler{
private:
	explicit Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs){
		_used_threads_id.push(FIRST_ID);
		_largest_thread_id = FIRST_ID;
		_create_thread();
	}
	const int _quantum_usecs;
	int _largest_thread_id;
	priority_queue <int, vector<int>, greater<int>> _used_threads_id;
	map<int, Thread> _active_threads;
	int _create_thread();
	int _generate_thread_id();
public:
	Scheduler(Scheduler const&) = delete;
	void operator=(Scheduler const&) = delete;
	static Scheduler& getInstance(int quantum_usecs = 0){
		static Scheduler instance(quantum_usecs);
		return instance;
	}
};

int Scheduler::_generate_thread_id()
{
	if(!_used_threads_id.empty()){
	//There's an available used id.
		int id = _used_threads_id.top();
		_used_threads_id.pop();
		return id;
	}
	//No used ID, generate new ID.
	return _largest_thread_id;
}

int Scheduler::_create_thread()
{
	int thread_id = _generate_thread_id();
	_active_threads[thread_id] = Thread();
	return 0;
}


int uthread_init(int quantum_usecs){
	Scheduler::getInstance(quantum_usecs);
	return 0;
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