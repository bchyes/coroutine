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
//int son[MAX_COROUTINE];
//int father[MAX_COROUTINE];
//int pthread_son[MAX_PTHREAD];
//int pthread_father[MAX_COROUTINE];
int retval[MAX_COROUTINE];
int fin_co[MAX_COROUTINE];
int now_co_id = 0;
int now_main_co_id = 0;

void refresh_main_context(struct coroutine *buf){
    main_context[buf -> id] = buf;
}
void refresh_context(struct coroutine *buf){
    if (buf -> id == now_co_id){
        context[now_co_id++] = buf;
    } else {
        context[buf -> id] = buf;
    }
}
void co_yield(){
    assert(now_coroutine != NULL);
    int res = setjmp(now_coroutine -> context);
    refresh_context(now_coroutine);
    //debug();
    if (res == 0){
        int rad;
        while (1){
            rad = (rand()) % (now_co_id + now_main_co_id);
            if (rad < now_main_co_id){
                if (now_main_context[rad]){
                    now_coroutine = main_context[rad];
                    //printf("longjmp main:%d\n",now_coroutine -> id);
                    //printf("now_main_co_id:%d\n",now_main_co_id);
                    break;
                }
            } else {
                if (!fin_co[rad - now_main_co_id]) {
                    now_coroutine = context[rad - now_main_co_id];
                    break;
                }
            }
        }
        //printf("longjmp:%d\n",now_coroutine -> id);
        longjmp(now_coroutine->context,1);
    }
}
int co_getid(){
    return now_coroutine -> id;
}
int co_status(int cid){
    return context[cid] -> status;
}
int co_getret(int cid){
    if (context[cid] -> status != FINISHED) co_yield();
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
    now_coroutine -> status = FINISHED;
    printf("come to this area exit\n");
    while(1){
        co_yield();
    }
}
int co_start(int (*routine)(void)){
    struct coroutine *cur = (struct coroutine*) malloc(sizeof (struct coroutine));
    struct coroutine set_coroutine; //need local,else we will modify main_context?
    if (now_main_co_id) printf("\nmain co_id:%d\n",main_context[0] -> id);
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
        set_coroutine.id = now_main_co_id - 1;
        //pthread_son[now_main_co_id - 1] = now_co_id;
        //pthread_father[now_co_id] = now_main_co_id - 1;
        //father[now_co_id] = -1;
        //son[now_co_id] = -1;
        cur -> id = now_co_id;
        res = setjmp(set_coroutine.context);
        refresh_main_context(&(set_coroutine));
    } else if (!now_main_context[i]){
        main_or_not = 1;
        now_main_context[i] = 1;
        return_main_co_id[i] = now_co_id;
        set_coroutine.id = i;
        //pthread_son[i] = now_co_id;
        //pthread_father[now_co_id] = i;
        //father[i] = -1;
        //son[i] = -1;
        cur -> id = now_co_id;
        printf("now_co_id in above = %d\n",now_co_id);
        debug();
        res = setjmp(set_coroutine.context);
        refresh_main_context(&(set_coroutine));
        //debug();
        printf("\nmain co_id:%d\n",main_context[0] -> id);
    } else {
        printf("now_co_id = %d\n",now_co_id);
        if (now_main_co_id) printf("\nmain co_id:%d\n",main_context[0] -> id);
        set_coroutine.thread = i;
        //if (father[now_co_id]) {}
        set_coroutine.id = now_coroutine -> id;
        if (now_main_co_id) printf("\nmain co_id1:%d\n",main_context[0] -> id);
        cur -> id = now_co_id;
        if (now_main_co_id) printf("\nmain co_id2:%d\n",main_context[0] -> id);
        return_co_id[set_coroutine.id] = now_co_id;
        debug();
        res = setjmp(set_coroutine.context);
        refresh_context(&(set_coroutine));
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
    printf("\n");
}