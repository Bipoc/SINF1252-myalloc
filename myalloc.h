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

