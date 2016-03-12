#ifndef _TESTS_H_
#define _TESTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <CUnit/CUnit.h>

void setHeapLimitAtLaunch();

void printMem();

void checkSize();

int isFullOfZeros(void* address, size_t size);

#endif
