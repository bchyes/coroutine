// ? Loc here: header modification to adapt pthread_setaffinity_np
#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <utmpx.h>
#include <sched.h>

void *thread1(void* dummy){
    assert(sched_getcpu() == 0);
    return NULL;
}

void *thread2(void* dummy){
    assert(sched_getcpu() == 1);
    return NULL;
}
int main(){
    pthread_t pid[2];
    int i;
    // ? LoC: Bind core here
    cpu_set_t cpuset[2];
    CPU_ZERO(&cpuset[0]);
    CPU_SET(0,&cpuset[0]);
    CPU_ZERO(&cpuset[1]);
    CPU_SET(1,&cpuset[1]);
    for(i = 0; i < 2; ++i){
        // 1 Loc code here: create thread and save in pid[2]
    }
    pthread_create(&pid[0],NULL,thread1,NULL);
    pthread_setaffinity_np(pid[0], sizeof(cpu_set_t), &cpuset[0]);
    pthread_create(&pid[1],NULL,thread2,NULL);
    pthread_setaffinity_np(pid[1], sizeof(cpu_set_t), &cpuset[1]);
    //int ret = pthread_getaffinity_np(pid[1], sizeof(cpu_set_t), &cpuset[1]);
    //printf("%d\n",ret);
    for(i = 0; i < 2; ++i){
        // 1 Loc code here: join thread
        pthread_join(pid[i],NULL);
    }
    return 0;
}
