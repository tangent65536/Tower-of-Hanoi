#!/bin/bash

gcc -c -o util/stack.o util/stack.c
gcc -o hanoi.elf hanoi.c util/stack.o -s
