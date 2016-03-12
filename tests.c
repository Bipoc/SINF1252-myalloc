#include "tests.h"
#include "myalloc.h"

extern int debugLevel;

static void* heapLimitAtLaunch;
extern size_t memSize;

void setHeapLimitAtLaunch()
{
	heapLimitAtLaunch = sbrk(0);
}

void printMem()
{
	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock;

	printf("start printing memory :\n");
	printf("\t\tallocated\tSize(block)\tCumulated Size\tAddress(header)\tisFullofZeros\n");
	int numBlock=0;
	int sumSize=0;
	while (currentAddress < heapLimitAtLaunch + memSize)
	{
		numBlock++;
		currentBlock = (block_header*)currentAddress;
		sumSize += HEADER_SIZE + currentBlock->size;
		if (currentBlock->alloc == 1)
			printf("\t%d : \tyes\t\t %d", numBlock, currentBlock->size);
		else
			printf("\t%d : \tno\t\t %d", numBlock, currentBlock->size);
		printf("\t\t%d\t\t%p", sumSize, currentAddress);
		if (isFullOfZeros(currentAddress + HEADER_SIZE, currentBlock->size))
			printf("\tyes");
		else
			printf("\tno");
		printf("\n");
		currentAddress += HEADER_SIZE + currentBlock->size;
	}
	printf("\n");
}

void printBytes()
{
	char* charAddress = (char*)heapLimitAtLaunch;
	int i=0;
	while ((void*)charAddress+i < heapLimitAtLaunch + memSize)
	{
		if (i%4 == 0)
			printf("\n");
		i++;
		printf("%d ", *(charAddress+i));
	}

	printf("\n");
}

void checkSize()
{
	void* currentAddress = heapLimitAtLaunch;
	size_t counted = 0;

	while (currentAddress < heapLimitAtLaunch + memSize)
	{

		block_header* currentBlock = (block_header*)currentAddress;	
		counted += currentBlock->size + HEADER_SIZE;

		currentAddress += HEADER_SIZE + currentBlock->size;
	}

	CU_ASSERT(counted == memSize);
}

void checkCalloc()
{
	void* currentAddress = heapLimitAtLaunch;
	int counted = 0;

	while (currentAddress < heapLimitAtLaunch + memSize)
	{

		block_header* currentBlock = (block_header*)currentAddress;	
		counted = counted && isFullOfZeros(currentAddress + HEADER_SIZE, currentBlock->size);

		currentAddress += HEADER_SIZE + currentBlock->size;
	}

	CU_ASSERT(counted == 0);
}

int isFullOfZeros(void* address, size_t size)
{
	char* charAddress = (char*)address;
	size_t i;
	for (i=0; i<size; i++)
	{
		if (*(charAddress+i)!=0)
			return 0;
	}

	return 1;
}

void resetMem()
{
	block_header* firstBlock = (block_header*)heapLimitAtLaunch;
	firstBlock->size = memSize - HEADER_SIZE;
	firstBlock->zero = 0;
	firstBlock->alloc = 0;
}

void fullBlockWithOnes(void* addressBlock)
{
	block_header* block = (block_header*)addressBlock;
	char* charAddress = (char*)addressBlock;
	size_t size = block->size;

	size_t i;
	for (i=0; i<size; i++)
		*(charAddress + HEADER_SIZE + i) = 1;
}

int initCallocTests()
{
	fullBlockWithOnes(heapLimitAtLaunch);

	mycalloc(4); void* p2=mycalloc(8); mycalloc(4); mycalloc(8);

	myfree(p2);

	mycalloc(4); mycalloc(0);

	return 0;
}

int finishCallocTests()
{
	resetMem();

	return 0;
}
