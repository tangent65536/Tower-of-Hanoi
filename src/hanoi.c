/**
 * Copyright (c) 2021, Tangent65536. All rights reserved.
 * 
 * An iterative solution for Tower of Hanoi implemented in C utilizing stack and concepts of binary tree traversal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/stack.h"

#define umaxof(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) |  (0xFULL << ((sizeof(t) * 8ULL) - 4ULL)))

#define uint_hanoi uint8_t
#define int_hanoi int16_t

#define false 0
#define true 1

typedef struct hanoi_pillar
{
  Stack *rings;
  char *name;
}
HanoiPillar;

typedef struct hanoi_platform
{
  HanoiPillar *start;
  HanoiPillar *middle;
  HanoiPillar *end;
}
HanoiPlatform;

typedef struct hanoi_ring
{
  uint_hanoi size;
}
HanoiRing;

enum TraverseNodeStatus
{
  PUSH_LEFT, // Push left branch into movement stack
  PUSH_RIGHT // Execute (move the corresponding ring), pop self, and push right branch into movement stack.
};

typedef struct hanoi_move
{
  HanoiPlatform cache_platform;
  HanoiRing ring;
  enum TraverseNodeStatus state;
} HanoiMove;

HanoiPillar *create_pillar(char *name)
{
  HanoiPillar *ret = malloc(sizeof(HanoiPillar));
  if(ret)
  {
    ret->name = malloc(strlen(name) + 1);
    if(!(ret->name))
    {
      free(ret);
      return NULL;
    }
    strcpy(ret->name, name);

    ret->rings = stack_construct();
    if(!(ret->rings))
    {
      free(ret->name);
      free(ret);
      return NULL;
    }
  }
  return ret;
}

void destruct_pillar(HanoiPillar *pillar)
{
  stack_destruct(pillar->rings, true);
  free(pillar->name);
  free(pillar);
}

uint64_t hanoi_steps(uint_hanoi lvl)
{
  HanoiPlatform platform;
  uint64_t ret = 0;
  HanoiRing *ring;
  HanoiMove *move, *cache_move;
  Stack *hanoi_movement_stack = stack_construct();
  int_hanoi current_lvl = lvl - 1, i;

  if(!lvl)
  {
    return 0;
  }

  /* Create platform */
  platform.start = create_pillar("A");
  platform.middle = create_pillar("B");
  platform.end = create_pillar("C");

  /* Init puzzle */
  for(i = current_lvl ; i >= 0 ; i--)
  {
    ring = malloc(sizeof(HanoiRing));
    ring->size = i;
    stack_push(platform.start->rings, ring);
  }

  /* Init movement stack by pushing the "root node" into it */
  move = malloc(sizeof(HanoiMove));
  move->cache_platform = platform;
  move->ring.size = current_lvl;
  move->state = PUSH_LEFT;
  stack_push(hanoi_movement_stack, move);

  /* Preform moves */
  while(move = stack_peek(hanoi_movement_stack))
  {
    switch(move->state)
    {
      case PUSH_LEFT:
      {
        if(move->ring.size > 0)
        {
          cache_move = malloc(sizeof(HanoiMove));
          cache_move->cache_platform.start = move->cache_platform.start;
          cache_move->cache_platform.middle = move->cache_platform.end;
          cache_move->cache_platform.end = move->cache_platform.middle;
          cache_move->ring.size = move->ring.size - 1;
          cache_move->state = PUSH_LEFT;
          stack_push(hanoi_movement_stack, cache_move);
        }
        move->state = PUSH_RIGHT;
        break;
      }
      case PUSH_RIGHT:
      {
        /* Move the ring */
        ring = stack_pop(move->cache_platform.start->rings);
        if(!ring)
        {
          fprintf(stderr, "Error while getting the ring to move! (Expecting movement of ring %d from %s to %s)\n", move->ring.size, move->cache_platform.start->name, move->cache_platform.end->name);
          return 0;
        }

        if(ring->size != move->ring.size)
        {
          fprintf(stderr, "Size mismatch for current state! (Expecting: %d, Actual: %d)\n", move->ring.size, ring->size);
        }

        fprintf(stdout, "Moving ring %d from %s to %s.\n", ring->size, move->cache_platform.start->name, move->cache_platform.end->name);
        stack_push(move->cache_platform.end->rings, ring);
        ret++;

        /* Remove this move from the stack since it's done */
        move = stack_pop(hanoi_movement_stack);

        /* Push the right branch into movement stack */
        if(move->ring.size > 0)
        {
          cache_move = malloc(sizeof(HanoiMove));
          cache_move->cache_platform.start = move->cache_platform.middle;
          cache_move->cache_platform.middle = move->cache_platform.start;
          cache_move->cache_platform.end = move->cache_platform.end;
          cache_move->ring.size = move->ring.size - 1;
          cache_move->state = PUSH_LEFT;
          stack_push(hanoi_movement_stack, cache_move);
        }

        /* Free memory */
        free(move);

        break;
      }
      default:
      {
        fprintf(stderr, "Bad movement state!");
      }
    }
  }
  stack_destruct(hanoi_movement_stack, false);

  /* Print outputs below. */
  fprintf(stdout, "\n");

  fprintf(stdout, "Rings on pillar %s (top to bottom): ", platform.start->name);
  while(ring = stack_pop(platform.start->rings))
  {
    fprintf(stdout, "%d ", ring->size);
  }
  fprintf(stdout, "\n");
  destruct_pillar(platform.start);

  fprintf(stdout, "Rings on pillar %s (top to bottom): ", platform.middle->name);
  while(ring = stack_pop(platform.middle->rings))
  {
    fprintf(stdout, "%d ", ring->size);
  }
  fprintf(stdout, "\n");
  destruct_pillar(platform.middle);

  fprintf(stdout, "Rings on pillar %s (top to bottom): ", platform.end->name);
  while(ring = stack_pop(platform.end->rings))
  {
    fprintf(stdout, "%d ", ring->size);
  }
  fprintf(stdout, "\n");
  destruct_pillar(platform.end);

  fprintf(stdout, "\n");

  return ret;
}

int main(int argc, char *argv[])
{
  uint64_t lvl;
  char *end_ptr;
  if(argc < 1)
  {
    fprintf(stderr, "Bad program call.\n");
  }
  else if(argc < 2)
  {
    fprintf(stderr, "Usage: %s [0 < level < %lld]\n", argv[0], umaxof(uint_hanoi));
  }
  else
  {
    lvl = strtoul(argv[1], &end_ptr, 10);
    if(*end_ptr)
    {
      fprintf(stderr, "Level must be a positive integer.\n");
    }
    else if(lvl > umaxof(uint_hanoi))
    {
      fprintf(stderr, "Level must be a positive integer less than %lld.\n", umaxof(uint_hanoi));
    }
    else if(lvl == 0)
    {
      fprintf(stdout, "There's nothing to solve with just a solid plate with 3 fixed sticks.\n");
    }
    else
    {
      fprintf(stdout, "Total steps: %ld\n", hanoi_steps(lvl));
    }
  }

  return 0;
}
