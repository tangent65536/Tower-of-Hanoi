#include <inttypes.h>

#include "linkednode.h"

#ifndef _STACK_H_

typedef struct stack
{
    struct node *top;
    uint32_t size;
} Stack;

Stack * stack_construct();
void stack_destruct(Stack *, uint8_t);
uint8_t stack_push(Stack *, void *);
void * stack_pop(Stack *);
void * stack_peek(Stack *);

#define _STACK_H_
#endif

