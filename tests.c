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

void checkSize()
{
	void* currentAddress = heapLimitAtLaunch;
	size_t counted = 0;

	while (currentAddress < heapLimitAtLaunch + memSize)
	{

		block_header* currentBlock = (block_header*)currentAddress;	
		counted += currentBlock->size + HEADER_SIZE;
	}

	//CU_ASSERT_TRUE(counted == memSize);
}

int isFullOfZeros(void* address, size_t size)
{
	char* charAddress = (char*)address;
	int i;
	for (i=0; i<size; i++)
		if (*(charAddress+i)!=0)
			return 1==0;
	return 1==1;
}
