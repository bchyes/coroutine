// ? Loc here: header modification to adapt pthread_cond_t
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXTHREAD 10
// declare cond_variable: you may define MAXTHREAD variables
pthread_cond_t cond[MAXTHREAD]; 
pthread_mutex_t mutex;
// ? Loc in thread1: you can do any modification here, but it should be less than 20 Locs
void *thread1(void* dummy){
    pthread_mutex_lock(&mutex);
	if (*((int*) dummy) < MAXTHREAD - 1)
		pthread_cond_wait(&cond[*((int*) dummy)], &mutex);
    int i;
    printf("This is thread %d!\n", *((int*) dummy));
    for(i = 0; i < 20; ++i){
        printf("H");
        printf("e");
        printf("l");
        printf("l");
        printf("o");
        printf("W");
        printf("o");
        printf("r");
        printf("l");
        printf("d");
        printf("!");
    }
    if (*((int*) dummy) > 0)
    	pthread_cond_signal(&cond[*((int*) dummy) - 1]);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(){
    pthread_t pid[MAXTHREAD];
    int i;
    // ? Locs: initialize the cond_variables
    pthread_cond_init(&cond[i],NULL);
    for(i = 0; i < MAXTHREAD; ++i){
        int* thr = (int*) malloc(sizeof(int)); 
        *thr = i;
        // 1 Loc here: create thread and pass thr as parameter
        pthread_create(&pid[i],NULL,thread1,(void *) thr); 
    }
    for(i = 0; i < MAXTHREAD; ++i)
        // 1 Loc here: join thread
        pthread_join(pid[i],NULL);
    return 0;
}
