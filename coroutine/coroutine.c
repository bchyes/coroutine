/* YOUR CODE HERE */
#include "coroutine.h"
#include <pthread.h>
#define alignment16(a) ((a) & (~(16 - 1)))
#define MAX_COROUTINE 40000
#define MAX_PTHREAD 400
struct coroutine* now_coroutine[MAX_PTHREAD];
struct coroutine* context[MAX_COROUTINE];
struct coroutine* main_context[MAX_PTHREAD];
long main_context_thread[MAX_PTHREAD]; //pthread_self()
int now_main_context[MAX_PTHREAD]; //to know whether in main
int return_main_co_id[MAX_PTHREAD];//co_id return to main.c
int return_co_id[MAX_COROUTINE];//co_id return to main.c
int retval[MAX_COROUTINE];
int fin_co[MAX_COROUTINE];
int now_co_id = 0;
int now_main_co_id = 0;
int debug_now_co_id = 0;
pthread_mutex_t mutex; 

void refresh_main_context(struct coroutine *buf){
    main_context[buf -> id] = buf;
}
void refresh_context(struct coroutine *buf){
    context[buf -> id] = buf;
}
void co_yield(){
    int origin_main = 0;
    int i;
    for (i = 0;i < now_main_co_id;i++){
        if (main_context_thread[i] == pthread_self()) break;
    }
    assert(now_coroutine[i] != NULL);
    if (now_coroutine[i] -> main_or_not){
        if (!now_main_context[i]){
            now_main_context[i] = 1; 
            origin_main = 1;
        }
    }
    int res = setjmp(now_coroutine[i] -> context);
    if (!(now_coroutine[i] -> main_or_not)){
        refresh_context(now_coroutine[i]);
    } else {
        refresh_main_context(now_coroutine[i]);
    }
    if (res == 0){
        int rad;
        while (1){
            rad = (rand()) % (now_co_id + 1);//I forget to change this !!(change to debug_now_co_id when debug)
            if (!rad){
                if (now_main_context[i]){
                    now_coroutine[i] = main_context[i];
                    break;
                }
            } else {
                if (context[rad - 1] != NULL && !fin_co[rad - 1] && i == context[rad - 1] -> thread) {//now_co_id will change before we change context so we need know whether context is NULL
                    now_coroutine[i] = context[rad - 1];
                    break;
                }
            }
        }
        longjmp(now_coroutine[i] -> context,1);
    }
    if (origin_main) {
        now_main_context[i] = 0; 
    }
}
int co_getid(){
    int i;
    for (i = 0;i < now_main_co_id;i++){
        if (main_context_thread[i] == pthread_self()) break;
    }
    return now_coroutine[i] -> id;
}
int co_status(int cid){
    return context[cid] -> status;
}
int co_getret(int cid){
    while (context[cid] -> status != FINISHED) co_yield(); //need while but not if !!
    return retval[cid];
}
void co_waitall(){
    for (int i = 0;i < now_co_id;i++){
        co_wait(i);
    }
}
void wait(struct coroutine *coroutine_){
    while (coroutine_->status != FINISHED) co_yield();
    fin_co[coroutine_ -> id] = 1;
}

void co_wait(int cid){
    if (!fin_co[cid])// can't modify to context because there is a situation that fin_co is finished but context -> status is RUNNING??
        wait(context[cid]);
}

void exit_(){
    int retval_;
    asm volatile(
        "movl %%eax, %0;"
        : "=r"(retval_)
        : 
        : "%eax"
    );
    int i;
    for (i = 0;i < now_main_co_id;i++){
        if (main_context_thread[i] == pthread_self()) break;
    }
    retval[now_coroutine[i] -> id] = retval_;
    fin_co[now_coroutine[i] -> id] = 1;
    now_coroutine[i] -> status = FINISHED;
    while(1){
        co_yield();
    }
}
int co_start(int (*routine)(void)){
    struct coroutine *cur = (struct coroutine*) malloc(sizeof (struct coroutine));
    struct coroutine *set_coroutine = (struct coroutine*) malloc(sizeof (struct coroutine)); //need local,else we will modify main_context?
    cur -> func = routine;
    cur -> status = RUNNING;
    void *now = (void *)(alignment16((uintptr_t) cur->stack + STACK_SIZE));
    int i;
    int main_or_not = 0;
    int res;
    for (i = 0;i < now_main_co_id;i++){
        if (main_context_thread[i] == pthread_self()) break;
    }
    if (i == now_main_co_id){
        pthread_mutex_lock(&mutex);
        main_context_thread[now_main_co_id] = pthread_self();
        now_main_co_id++;
        main_or_not = 1;
        now_main_context[now_main_co_id - 1] = 1;
        return_main_co_id[now_main_co_id - 1] = now_co_id;
        set_coroutine -> id = now_main_co_id - 1;
        cur -> id = now_co_id;
        now_co_id++;
        cur -> thread = i;
        set_coroutine -> main_or_not = 1;
        pthread_mutex_unlock(&mutex);
        res = setjmp(set_coroutine -> context);
        refresh_main_context(set_coroutine);
    } else if (!now_main_context[i]){
        pthread_mutex_lock(&mutex);
        main_or_not = 1;
        now_main_context[i] = 1;
        return_main_co_id[i] = now_co_id;
        set_coroutine -> id = i;
        set_coroutine -> main_or_not = 1;
        cur -> id = now_co_id;
        now_co_id++;
        cur -> thread = i;
        pthread_mutex_unlock(&mutex);
        res = setjmp(set_coroutine -> context);
        refresh_main_context(set_coroutine);
    } else {
        pthread_mutex_lock(&mutex);
        set_coroutine -> thread = i;
        set_coroutine -> id = now_coroutine[i] -> id;
        set_coroutine -> main_or_not = 0;
        set_coroutine -> status = RUNNING;
        cur -> id = now_co_id;
        return_co_id[set_coroutine -> id] = now_co_id;
        now_co_id++;
        cur -> thread = i;
        pthread_mutex_unlock(&mutex);
        if (set_coroutine == NULL) printf("NULL\n");
        res = setjmp(set_coroutine -> context);
        refresh_context(set_coroutine);
    }
    if (res == 0){
        now_coroutine[i] = cur;
        refresh_context(now_coroutine[i]);
        asm volatile( 
            "movq %0, %%rsp;"
            "pushq %2;"
            "jmp *%1;"
            :
            : "b"(now), "d"(cur -> func), "c"(exit_)
            : "memory"
        );
    }
    if (main_or_not){
        for (i = 0;i < now_main_co_id;i++){
            if (main_context_thread[i] == pthread_self()) break;
        }
        now_main_context[i] = 0;
        return return_main_co_id[i];
    } else {
        return return_co_id[now_coroutine[i] -> id];
    }
}

void debug(){
    //We don't need to use debug_now_co_id because it has a little problem but we can use if as co_yield with now_co_id to know whether it is NULL or not
    pthread_mutex_lock(&mutex);
    int j;
    for (j = 0;j < now_co_id;j++){
        if (context[j] == NULL) break;
    }
    printf("\n");
    printf("debug\n");
    printf("return_main_co_id = ");
    for (int i = 0;i < now_main_co_id;i++){
        printf("%d ",return_main_co_id[i]);
    }
    printf("\n");
    printf("return_co_id = ");
    for (int i = 0;i < j;i++){
        printf("%d ",return_co_id[i]);
    }
    printf("\n");
    printf("finished:");
    for (int i = 0;i < j;i++){
        printf("%d ",fin_co[i]);
    }
    printf("\n");
    printf("main finished:");
    for (int i = 0;i < now_main_co_id;i++){
        printf("%d ",now_main_context[i]);
    }
    printf("\n");
    printf("main co_id:");
    for (int i = 0;i < now_main_co_id;i++){
        printf("%d ",main_context[i] -> id);
    }
    printf("\n");
    printf("ret val:");
    for (int i = 0;i < j;i++){
        printf("%d ",retval[i]);
    }
    printf("\n");
    printf("finished:");
    for (int i = 0;i < j;i++){
        printf("%d ",context[i] -> status);
    }
    printf("\n");
    printf("thread:");
    for (int i = 0;i < j;i++){
        printf("%d ",context[i] -> thread);
    }
    printf("\n");
    printf("\n");
    pthread_mutex_unlock(&mutex);
}
void debug_spc(int i){
    printf("%d\n",context[i] -> status);
    printf("%d\n",fin_co[i]);
    printf("%d\n",co_status(i));
}
void debug_context(){
     for (int i = 1;i < debug_now_co_id;i++){
        if (context[i] -> status != fin_co[i] + 1){
            printf("i :%d\n",i);
            debug();
            exit(0);
        }
        assert(context[i] -> status == fin_co[i] + 1);
    } 
}
void debug_now_coroutine(){
    pthread_mutex_lock(&mutex);
    printf("\ndebug now coroutine\n");
    for (int i = 0;i < now_main_co_id;i++){
        printf("%d ",now_coroutine[i]->id);
    }
    printf("\n");
    pthread_mutex_unlock(&mutex);
}