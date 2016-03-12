#ifndef _TESTS_H_
#define _TESTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <CUnit/CUnit.h>

#define MEM_SIZE_TESTS 40

void setHeapLimitAtLaunch();

void printMem();

void printBytes();

void checkSize();

void checkCalloc();

int isFullOfZeros(void* address, size_t size);

void resetMem();

void fullBlockWithOnes(void* addressBlock);

int initCallocTests();

int finishCallocTests();

#endif
