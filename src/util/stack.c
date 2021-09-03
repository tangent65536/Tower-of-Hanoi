#include <stdlib.h>
#include "stack.h"

Stack * stack_construct()
{
  Stack *ret = malloc(sizeof(Stack));
  if(ret)
  {
    ret->size = 0;
    ret->top = NULL;
  }
  return ret;
}

void stack_destruct(Stack *stack, uint8_t deep_delete)
{
  Node *top, *tmp;
  if(stack)
  {
    top = stack->top;
    while(top)
    {
      if(deep_delete)
      {
        free(top->data);
      }
      tmp = top;
      top = top->next;
      free(tmp);
    }
    free(stack);
  }
}

uint8_t stack_push(Stack *stack, void *data)
{
  Node *node;
  if(stack)
  {
    node = malloc(sizeof(Node));
    if(node)
    {
      node->data = data;
      node->next = stack->top;
      stack->top = node;
      stack->size++;
      return 1;
    }
  }
  return 0;
}

void * stack_pop(Stack *stack)
{
  void *ret;
  Node *node;
  if(stack)
  {
    node = stack->top;
    if(node)
    {
      ret = node->data;
      stack->top = node->next;
      free(node);
      stack->size--;
      return ret;
    }
  }
  return NULL;
}

void * stack_peek(Stack *stack)
{
  void *ret;
  if(stack && stack->top)
  {
    ret = stack->top->data;
    return ret;
  }
  return NULL;
}
