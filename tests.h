#ifndef _TESTS_H_
#define _TESTS_H_

#include <CUnit/CUnit.h>
void CU_basic_run_tests();//to avoid warning
void CU_basic_show_failures();

#define MEM_SIZE_TESTS 40

//start
//only one call and before any call to mymalloc or my calloc
void setHeapLimitAtLaunch();

//launch CUnit Tests
int cunitTests();

//print
void printMem();
void printBytes();

//tools
void resetMem();
void fullBlockWithOnes(void* addressBlock);

//basic tests : first tests with basic call
void testBasicFree();
void testBasicAllocAndFree();
void testBlockbyBlockBasic();
void mustFail();

//Free tests
void testBlockbyBlockFree();
void allocMustSuccess();

//Limits tests : test at the start and the end of memory
void testBlockbyBlockLimits();
void allocMustFail();

//calloc tests
void checkSize();
void checkCalloc();
int isFullOfZeros(void* address, size_t size);


//tests initiater/finisher
int initBasicTests();
int initFreeTests();
int initLimitsTests();
int initCallocTests();

int finishTests();

#endif
