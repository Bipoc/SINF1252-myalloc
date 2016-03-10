#ifndef _MYALLOC_H
#define _MYALLOC_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define HEADER_SIZE		4
#define BYTES_ALIGNMENT	4

typedef struct block_header
{
	unsigned int size : 29,
				 zero : 2,
				 alloc : 1;
}block_header;

void* mymalloc(size_t size);
void* mycalloc(size_t size);
void myfree(void* ptr);

void initHeapLimitAtLaunch();

/**
allocate the block at the address passed in argument for the size
passed in argument
if the size does not fit perfectly in the block we create a new block
in the rest of the free memory (at the end)
the size is supposed to be <= than the size of the block
note : this work only if HEADER_SIZE == BYTES_ALIGNEMENT
**/
void allocateBlock(void* blockAddress, size_t size);

/**
return the address of the block's header that best fit in memory
nbBytes is supposed to be >0
if NULL is returned, none address is convenient
if -1 is returned, sbrk fail to give the heapLimit (possible change of that behavior)
!it doesn't allocate the block it just give is position!
group sequence of free block and reduce heaplimit if possible (possible change of that behavior)
**/
void* bestFit(size_t nbBytes);

/**
increase the size of the block if possible
	if the block is followed by other free block(s) and is itself free
**/
void updateBlock(block_header* block);

/**
try to decrease the heapLimit if lastBlock is not allocated
**/
void decreaseHeapLimit(block_header* lastBlock);

#endif
