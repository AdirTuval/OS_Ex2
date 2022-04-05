#include <cstdio>
#include <cstdlib>

//
// Created by adirt on 05/04/2022.
//
#include "uthreads.h"
void thread1(){
    int i = uthread_get_total_quantums();
    int j = 300;
    for(;;){
        if(i < uthread_get_total_quantums()){
            i = uthread_get_total_quantums();
        }
        if(i == uthread_get_total_quantums()){

            printf("thread1 : Current running thread: %d. Number of quantoms for this thread: %d. Total quantoms: %d. my j is: %d.\n",uthread_get_tid(),
                   uthread_get_quantums(uthread_get_tid()), uthread_get_total_quantums(), j);
            if(i == 25){
                uthread_resume(2);
                printf("Thread 2 has been resumed by thread 1.\n");
            }
            if(i == 17){
                printf("Thread 1 is going to sleep for 1 quantums.\n");
                uthread_sleep(1);
            }
            fflush(stdout);
            i++;
            j++;
        }
    }
}
void thread2(){
    int i = uthread_get_total_quantums();
    int j = 200;
    for(;;){
        if(i < uthread_get_total_quantums()){
            i = uthread_get_total_quantums();
        }
        if(i == uthread_get_total_quantums()){
            j++;
            i++;
            printf("thread2 : Current running thread: %d. Number of quantoms for this thread: %d. Total quantoms: %d. my j is: %d\n",uthread_get_tid(),
                   uthread_get_quantums(uthread_get_tid()), uthread_get_total_quantums(), j);
            fflush(stdout);
        }
    }
}
int main(){
    uthread_init(100000);
    int i = 1;
    int j = 100;
    for(;;){
        if(i < uthread_get_total_quantums()){
            i = uthread_get_total_quantums();
        }
        if(i == uthread_get_total_quantums()){
            printf("thread0 : Current running thread: %d. Number of quantoms for this thread: %d. Total quantoms: %d. my j is: %d\n",uthread_get_tid(),
                   uthread_get_quantums(uthread_get_tid()), uthread_get_total_quantums(), j);
            if(i == 3){
                uthread_spawn(&thread1);
            }
            if(i == 11){
                uthread_spawn(&thread2);
            }
            if(i == 14){
                uthread_block(2);
                printf("Thread 2 has been blocked by thread 0.\n");
            }
            i++;
            j++;
        }

    }
}
