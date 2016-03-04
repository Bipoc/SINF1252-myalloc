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

void dropHeapLimit(void* ptrLastFreedBlock);

void initHeapLimitAtLaunch();

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
