/* YOUR CODE HERE */
#ifndef COROUTINE_H
#define COROUTINE_H

#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define STACK_SIZE 4096
typedef long long cid_t;
#define MAXN (50000)
#define UNAUTHORIZED (-1)
#define FINISHED (2)
#define RUNNING (1)

struct coroutine{
    int id;
    int (*func)(void);
    int status;
    jmp_buf context;
    uint8_t stack[STACK_SIZE];
};

int co_start(int (*routine)(void));
int co_getid();
int co_getret(int cid);
void co_yield();
void co_waitall();
void co_wait(int cid);
int co_status(int cid);

#endif
