//
// Created by adirt on 01/04/2022.
//
#include <cstdio>
#include <cassert>
#include "uthreads.h"
#include <bits/stdc++.h>
#include <csetjmp>
#define FIRST_ID 0
#define SUCCESS 0
#define FAILURE -1
using namespace std;
#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
	address_t ret;
	asm volatile("xor    %%fs:0x30,%0\n"
				 "rol    $0x11,%0\n"
	: "=g" (ret)
	: "0" (addr));
	return ret;
}
#endif

enum ThreadState {RUNNING, READY, BLOCKED, TERMINATED};

class Thread{
private:
	ThreadState _state;
	sigjmp_buf _env;
	char * _stack;
	const int _id;
public:
	explicit Thread(int id, thread_entry_point entry_point = nullptr) : _id(id), _state(ThreadState::READY), _env{0}{
		//TODO if id is unused remove it
		_stack = new char[STACK_SIZE];
		int ret_val = sigsetjmp(_env, 1); // TODO check if savemask == 1
		if(ret_val == 0){
			if(entry_point != nullptr){
				address_t pc = (address_t) entry_point;
				(_env->__jmpbuf)[JB_PC] = translate_address(pc);
				address_t sp = (address_t) _stack + STACK_SIZE - sizeof(address_t);
				(_env->__jmpbuf)[JB_SP] = translate_address(sp);
			}
			sigemptyset(&_env->__saved_mask);
		}
	}
	int get_id() const {return _id;}
	ThreadState get_state() const {return _state;}
	void set_state(ThreadState new_state) {_state = new_state;}
	~Thread(){
		delete[] _stack;
		printf("Thread deleted");
	}

};

class Scheduler{
private:
	explicit Scheduler(int quantum_usecs) : _quantum_usecs(quantum_usecs), _active_thread_id(FIRST_ID){
		_unused_threads_id.push(FIRST_ID);
		_largest_thread_id = FIRST_ID;
		create_thread(nullptr);
	}
	const int _quantum_usecs;
	int _largest_thread_id;
	int _active_thread_id;
	priority_queue <int, vector<int>, greater<int>> _unused_threads_id;
	unordered_map<int, Thread *> _active_threads;
	deque<Thread *> _ready_queue;
public:
	int create_thread(thread_entry_point entry_point);
	int terminate_thread(int tid);
	int _generate_thread_id();
	Scheduler(Scheduler const&) = delete;
	void operator=(Scheduler const&) = delete;
	int _erase_from_ready_queue(int tid);
	static Scheduler& getInstance(int quantum_usecs = 0){
		static Scheduler instance(quantum_usecs);
		return instance;
	}

	~Scheduler(){
		if(!_active_threads.empty()) {
			for(unordered_map<int,Thread *>::iterator it=_active_threads.begin() ; it!=_active_threads.end() ; it++){
				delete it->second;
			}
			_active_threads.clear();
		}
	}
};

int Scheduler::_generate_thread_id()
{
	if(!_unused_threads_id.empty()){
	//There's an available used id.
		int id = _unused_threads_id.top();
		_unused_threads_id.pop();
		return id;
	}
	//No used ID, generate new ID.
	return ++_largest_thread_id;
}

int Scheduler::create_thread(thread_entry_point entry_point)
{
	if(_active_threads.size() >= MAX_THREAD_NUM){
		return FAILURE;
	}
	try {
		int thread_id = _generate_thread_id();
		auto * new_thread = new Thread(thread_id, entry_point);
		_active_threads[thread_id] = new_thread;
		_ready_queue.push_back(new_thread);
		return thread_id;
	} catch (bad_alloc&){
		return FAILURE;
	}
}

int Scheduler::terminate_thread(int tid) {
	if(_active_threads.find(tid) == _active_threads.end()){
		return FAILURE;
	}
	//TODO if thread is running, then we should push new thread to running
	Thread * thread_to_terminate = _active_threads[tid];
	if(thread_to_terminate->get_state() == ThreadState::RUNNING){
		//run new thread
		_run_top_thread();
	}
	else if(thread_to_terminate->get_state() == ThreadState::READY){
		if(_erase_from_ready_queue(tid) == FAILURE){
		//TODO print error
			return FAILURE;
		}
	}
	_active_threads.erase(tid);
	_unused_threads_id.push(tid);
	delete thread_to_terminate;
	return SUCCESS;
}

int Scheduler::_erase_from_ready_queue(int tid) {
	for (deque<Thread *>::iterator it = _ready_queue.begin(); it != _ready_queue.end(); ++it){
		if((*it)->get_id() == tid){
			_ready_queue.erase(it);
			return SUCCESS;
		}
	}
	return FAILURE;
}


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

}
int uthread_resume(int tid);
int uthread_sleep(int num_quantums);
int uthread_get_tid();
int uthread_get_total_quantums();
int uthread_get_quantums(int tid);

int main(){
	Scheduler &t = Scheduler::getInstance(20);
	printf("%d",t._generate_thread_id());
	printf("%d",t._generate_thread_id());
	printf("%d",t._generate_thread_id());
	return 0;
}