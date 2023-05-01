/* YOUR CODE HERE */
#include "coroutine.h"
#define alignment16(a) ((a) & (~(16 - 1)))
#define MAX_COROUTINE 2000
#define NESTED_NUM 10
struct coroutine main_coroutine[NESTED_NUM];
int now_nest;
int nest_id[NESTED_NUM];
struct coroutine *now_coroutine;
struct coroutine* context[MAX_COROUTINE];
int retval[MAX_COROUTINE];
int fin_co[MAX_COROUTINE];
int now_co_id = 0;
struct coroutine* test;

void refresh_context(struct coroutine *buf){
    // int same = 0;
    // for (int i = 0;i < now_co_id;i++){
    //     if (context[i] == buf){
    //         same = 1;
    //         break;
    //     }
    // }
    // if (!same){
    //     context[now_co_id++] = buf;
    // }
    //printf("%d\n",now_co_id);
    if (buf -> id + 1 == now_co_id){
        context[now_co_id++] = buf;
    } else {
        context[buf -> id + 1] = buf;
    }
    printf("modify id %d in context\n",buf -> id + 1);
}
void co_yield(){
    assert(now_coroutine != NULL);
    int res = setjmp(now_coroutine -> context);
    refresh_context(now_coroutine);
    //if (now_coroutine -> id == 10) printf("-");
    printf("%d ?\n",now_co_id);
    if (res == 0){
        int rad;
        while (1){
            rad = (rand()) % now_co_id;
            if (!fin_co[rad]) break;
        }
        now_coroutine = context[rad];
        printf("%d\n",rad);
        if (rad == 26) {
            if (now_coroutine->context == test->context) printf("same\n");
            else printf("different\n");
        }
        if (rad == 26){
            now_coroutine = context[26];
        }
        longjmp(now_coroutine->context,1);
    }
}
int co_getid(){
    return now_coroutine -> id;
}
int co_status(int cid){
    return context[cid + 1] -> status;
}
int co_getret(int cid){
    if (context[cid + 1] -> status != FINISHED) co_yield();
    //printf("%d\n",retval[cid + 1]);
    return retval[cid + 1];
}
void co_waitall(){
    //printf("%d\n",now_co_id - 1);
    for (int i = 0;i < now_co_id - 1;i++){
        co_wait(i);
    }
}
void wait(struct coroutine *coroutine_){
    while (coroutine_->status != FINISHED) co_yield();
    // for (int i = 0;i < now_co_id;i++){
    //     if (context[i] == coroutine_){
    //         for (int j = i;j < now_co_id - 1;j++){
    //             context[i] = context[i + 1];
    //         }
    //         now_co_id--;
    //         break;
    //     }
    // }
    fin_co[coroutine_ -> id + 1] = 1;
    //free(coroutine_); ????
}

void co_wait(int cid){
    if (!fin_co[cid + 1])
        wait(context[cid + 1]);
}

void exit_(){
    int retval_;
    asm volatile(
        "movl %%eax, %0;"
        : "=r"(retval_)
        : 
        : "%eax"
    );
    //int retval2 = *((int *) retval);
    //printf("%d ",retval_);
    //printf("%d\n",now_coroutine -> id + 1);
    retval[now_coroutine -> id + 1] = retval_;
    now_coroutine -> status = FINISHED;
    printf("come to this area2\n");
    while(1){
        co_yield();
        //printf("come to this area3\n");
    }
    //printf("come to this area3\n");
}
int co_start(int (*routine)(void)){
    printf("%d\n",now_co_id - 1);
    struct coroutine *cur = (struct coroutine*) malloc(sizeof (main_coroutine));
    cur -> func = routine;
    cur -> status = RUNNING;
    void *now = (void *)(alignment16((uintptr_t) cur->stack + STACK_SIZE));
    //printf("nest:%d\n",now_nest);
    if (!now_nest) {
        main_coroutine[now_nest].id = -1;
    } else {
        main_coroutine[now_nest].id = nest_id[now_nest - 1];
    }
    now_nest++;
    int flag_co = 0;
    if (!now_co_id)
        nest_id[now_nest - 1] = now_co_id;
    else 
        nest_id[now_nest - 1] = now_co_id - 1;
    int res = setjmp(main_coroutine[now_nest - 1].context);    
    printf("SetJmp %d id and res = %d and now_nest = %d\n",main_coroutine[now_nest - 1].id, res,now_nest);
    while (1){
        refresh_context(&(main_coroutine[now_nest - 1]));
        //printf("res:%d\n",res);
        if (!res) break; else {
            //printf("nest:%d\n",now_nest);
            //printf("id:%d\n",now_coroutine -> id);
            now_nest--;
            flag_co = 1;
            if (!now_nest) break;
            now_coroutine = context[nest_id[now_nest - 1] + 1];
            //printf("id:%d\n",now_coroutine -> id);
            res = 0;
        }
    }
    // if (res) {
    //     now_nest--;
    //     if (now_nest){
    //         //printf("now_nest:%d\n",now_nest);
    //         int res = setjmp(main_coroutine[now_nest - 1].context);
    //         refresh_context(&(main_coroutine[now_nest - 1]));
    //     }
    // }
    //printf("%d\n",now_co_id);
    //int ret_id;
    for (int i=0;i<now_nest;i++){
        printf("%d? ",nest_id[i]);
    }
    printf("\n");
    if (!flag_co){
        cur -> id = now_co_id - 1;
        //ret_id = cur -> id;
        now_coroutine = cur;
        refresh_context(now_coroutine);
        printf("come to this area1 and cur->id is %d\n",cur->id);
        //printf("%s\n",context[cur->id + 1]->context);
        if (cur -> id + 1 == 26) test = (context[cur -> id + 1]);
        asm volatile(
            "movq %0, %%rsp;"
            "pushq %1;"
            "jmp *%2;"
            :
            : "b"(now),"c"(exit_),"d"(cur -> func)
            : "memory"
        );
    }
    //printf("come to this area4\n");
    printf("return nest id %d and nest = %d\n",nest_id[now_nest],now_nest);
    return nest_id[now_nest];
}