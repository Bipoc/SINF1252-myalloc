#ifndef _MYALLOC_H_
#define _MYALLOC_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> // to have access to function "memset"

#define HEADER_SIZE		4
#define BYTES_ALIGNMENT	4

typedef struct block_header
{
	unsigned int size : 29,
				 zero : 2,
				 alloc : 1;
}block_header;

void* mymalloc(size_t size);
void* mycalloc(size_t nmemb, size_t size);
void myfree(void* ptr);

/**
tests if the address pointed by the pointer is an effective block
**/
int isBlock(void* ptr);

/**
initializes the heap on the first call to this function, by calling sbrk
if memSize is negative or sbrk return -1, it stop the program 
**/
void initHeapLimitAtLaunch();

/**
allocates the block at the address given as a parameter for the size
given as a parameter
if the size does not fit perfectly the current block, we create a new free block
in the rest of the free memory (at the end)
the size is supposed to be greater or equal to the size of the block
note : the behavior is not guaranteed if HEADER_SIZE != BYTES_ALIGNEMENT
**/
void allocateBlock(void* blockAddress, size_t size);

/**
returns the address of the block's header that best fit in memory
nbBytes is supposed to be greater than 0
if NULL is returned, there isn't any convenient address available
!it doesn't allocate the block it just give its position!
the function also groups following sequences of free block
**/
void* bestFit(size_t nbBytes);

/**
increases the size of the block if the block
is followed by other free block(s) and is itself free
**/
void updateBlock(block_header* block);

#endif
