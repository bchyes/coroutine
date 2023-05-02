/* YOUR CODE HERE */
#include "coroutine.h"
#include <pthread.h>
#define alignment16(a) ((a) & (~(16 - 1)))
#define MAX_COROUTINE 20000
#define MAX_PTHREAD 200
struct coroutine *now_coroutine;
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

void refresh_main_context(struct coroutine *buf){
    if (buf -> id == 0){
        //printf("modify main 0:\n");
    }
    main_context[buf -> id] = buf;
}
void refresh_context(struct coroutine *buf){
    if (buf -> id == 0){
        //printf("modify 0:\n");
    }
    if (buf -> id == now_co_id){
        context[now_co_id++] = buf;
    } else {
        context[buf -> id] = buf;
    }
}
void co_yield(){
    assert(now_coroutine != NULL);
    //printf("coroutine id = %d\n",now_coroutine -> id);
    //printf("coroutine main_or_not = %d\n",now_coroutine -> main_or_not);
    //debug();
    int origin_main = 0;
    int i;
    for (i = 0;i < now_main_co_id;i++){
        if (main_context_thread[i] == pthread_self()) break;
    }
    if (now_coroutine -> main_or_not){
        if (!now_main_context[i]){
            now_main_context[i] = 1; 
            origin_main = 1;
        }
    }
    int res = setjmp(now_coroutine -> context);
    if (!(now_coroutine -> main_or_not)){
        refresh_context(now_coroutine);
    } else {
        refresh_main_context(now_coroutine);
    }
    //debug();
    if (res == 0){
        int rad;
        while (1){
            rad = (rand()) % (now_co_id + 1);
            if (!rad){
                if (now_main_context[i]){
                    now_coroutine = main_context[i];
                    //printf("longjmp main:%d\n",now_coroutine -> id);
                    //printf("now_main_co_id:%d\n",now_main_co_id);
                    break;
                }
            } else {
                if (!fin_co[rad - 1] && i == context[rad - 1] -> thread) {
                    now_coroutine = context[rad - 1];
                    break;
                }
            }
        }
        //printf("longjmp:%d\n",now_coroutine -> id);
        longjmp(now_coroutine->context,1);
    }
    if (origin_main) {
        now_main_context[i] = 0; 
    }
}
int co_getid(){
    //printf("getid = %d\n",now_coroutine -> id);
    return now_coroutine -> id;
}
int co_status(int cid){
    //printf("status = %d\n",context[cid] -> status);
    //debug();
    return context[cid] -> status;
}
int co_getret(int cid){
    //printf("getret : cid %d\n",cid);
    //printf("getret : now_cid %d\n",now_coroutine -> id);
    /* int i;
    for (i = 0;i < now_main_co_id;i++){
        if (main_context_thread[i] == pthread_self()) break;
    }
    now_main_context[i] = 1; */ //need to do this in co_yield
    while (context[cid] -> status != FINISHED) co_yield(); //need while but not if !!
    //now_main_context[i] = 0;
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
    if (!fin_co[cid])
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
    retval[now_coroutine -> id] = retval_;
    fin_co[now_coroutine -> id] = 1;
    now_coroutine -> status = FINISHED;
    //printf("come to this area exit\n");
    while(1){
        co_yield();
    }
}
int co_start(int (*routine)(void)){
    struct coroutine *cur = (struct coroutine*) malloc(sizeof (struct coroutine));
    struct coroutine *set_coroutine = (struct coroutine*) malloc(sizeof (struct coroutine)); //need local,else we will modify main_context?
    //if (now_main_co_id) printf("\nmain co_id:%d\n",main_context[0] -> id);
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
        main_context_thread[now_main_co_id++] = pthread_self();
        main_or_not = 1;
        now_main_context[now_main_co_id - 1] = 1;
        return_main_co_id[now_main_co_id - 1] = now_co_id;
        set_coroutine -> id = now_main_co_id - 1;
        cur -> id = now_co_id;
        set_coroutine -> main_or_not = 1;
        //debug();
        res = setjmp(set_coroutine -> context);
        refresh_main_context(set_coroutine);
    } else if (!now_main_context[i]){
        main_or_not = 1;
        now_main_context[i] = 1;
        return_main_co_id[i] = now_co_id;
        set_coroutine -> id = i;
        set_coroutine -> main_or_not = 1;
        cur -> id = now_co_id;
        //printf("now_co_id in above = %d\n",now_co_id);
        //debug();
        res = setjmp(set_coroutine -> context);
        refresh_main_context(set_coroutine);
        //debug();
        //printf("\nmain co_id:%d\n",main_context[0] -> id);
    } else {
        //printf("now_co_id = %d\n",now_co_id);
        //if (now_main_co_id) printf("\nmain co_id:%d\n",main_context[0] -> id);
        set_coroutine -> thread = i;
        set_coroutine -> id = now_coroutine -> id;
        set_coroutine -> main_or_not = 0;
        set_coroutine -> status = RUNNING; //!
        //if (now_main_co_id) printf("\nmain co_id1:%d\n",main_context[0] -> id);
        cur -> id = now_co_id;
        //if (now_main_co_id) printf("\nmain co_id2:%d\n",main_context[0] -> id);
        //printf("now_coroutine = %d\n",set_coroutine -> id);
        return_co_id[set_coroutine -> id] = now_co_id;
        //debug();
        res = setjmp(set_coroutine -> context);
        refresh_context(set_coroutine);
    }
    if (res == 0){
        now_coroutine = cur;
        refresh_context(now_coroutine);
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
        return return_co_id[now_coroutine -> id];
    }
}

void debug(){
    printf("\n");
    printf("debug\n");
    printf("return_main_co_id = ");
    for (int i = 0;i < now_main_co_id;i++){
        printf("%d ",return_main_co_id[i]);
    }
    printf("\n");
    printf("return_co_id = ");
    for (int i = 0;i < now_co_id;i++){
        printf("%d ",return_co_id[i]);
    }
    printf("\n");
    printf("finished:");
    for (int i = 0;i < now_co_id;i++){
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
    for (int i = 0;i < now_co_id;i++){
        printf("%d ",retval[i]);
    }
    printf("\n");
    printf("finished:");
    for (int i = 0;i < now_co_id;i++){
        printf("%d ",context[i] -> status);
    }
    printf("\n");
    printf("\n");
}