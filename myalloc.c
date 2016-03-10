#include "myalloc.h"

static void* heapLimitAtLaunch = 0;
static void* actualHeapLimit = 0;

extern int debugLevel;
extern size_t memSize;

void* mymalloc(size_t size)
{
	initHeapLimitAtLaunch();

	if (size <=0)
		return NULL;

	size_t nbBytes = size/4;
	if (size%4 > 0)
		nbBytes++;
	nbBytes *= 4;

	if (debugLevel == 2)
		printf("\t[[[ mymalloc==> Allocates %d bytes (+ %d header)...\n", (int) nbBytes, HEADER_SIZE);


	void* blockAddress = bestFit(nbBytes);

	if ((long) blockAddress == -1)
		return NULL;

	if (blockAddress == NULL)
		return NULL;

	allocateBlock(blockAddress, size);

	return blockAddress + HEADER_SIZE;
}

void* mycalloc(size_t size)
{
	short* address = mymalloc(size);//sizeof short always ==1?
	if (address == NULL)
		return NULL;

	for (int i=0; i<size; i++)
		*(address+i) = 0;

	return address;
}

void myfree(void* ptr)
{
	if (ptr == NULL)
		return;

	isReleasable(ptr);

	block_header* ptrHeader = (block_header*) (ptr-HEADER_SIZE);
	ptrHeader->alloc = 0;
}

void isReleasable(void* ptr)
{
	if (ptr == NULL)
		exit(EXIT_FAILURE)//TODO change?

	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock;

	while (currentAddress != actualHeapLimit)
	{
		currentBlock = (block_header*)currentAddress;

		if (currentAddress == ptr)
			return;

		currentAddress += HEADER_SIZE + currentBlock->size;
	}

	exit(EXIT_FAILURE);//TODO change?
}

void allocateBlock(void* blockAddress, size_t size)
{
	if (blockAddress == NULL)
		return;

	block_header* block = (block_header*)blockAddress;

	if (block->size < size)
		return;

	if (block->size != size)
	{
		void* newBlockAddress = blockAddress + HEADER_SIZE + size;
		block_header* newBlock = (block_header*)newBlockAddress;
		newBlock->size = block->size - size - HEADER_SIZE;
		newBlock->zero = 0;
		newBlock->alloc = 0;
	}

	block->alloc = 1;
}

void initHeapLimitAtLaunch()
{
	static int isInit = 0;
	if (!isInit)
	{
		heapLimitAtLaunch = sbrk(0);
		void *answer = sbrk(memSize);
		if ((long)answer == -1)
			exit(EXIT_FAILURE);//TODO add message with errno
		
		actualHeapLimit = heapLimitAtLaunch + memSize;

		isInit = 1;
	}
}

void* bestFit(size_t nbBytes)
{
	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock;

	void* bestFitAddress=NULL;
	size_t sizeBestFit;

	while (currentAddress < actualHeapLimit)
	{

		currentBlock = (block_header*)currentAddress;

		updateBlock(currentBlock);

		if (currentBlock->alloc == 0)
		{
			if (currentBlock->size >= nbBytes && (bestFitAddress == NULL || currentBlock->size < sizeBestFit))//add shortcut if size==nbBytes? break program behavior but increase speed
			{
				bestFitAddress = currentAddress;
				sizeBestFit = currentBlock->size;
			}
		}

		currentAddress += currentBlock->size + HEADER_SIZE;
	}

	return bestFitAddress;
}

void updateBlock(block_header* block)
{
	if (block == NULL || block->alloc == 1)
		return;

	size_t totalSize = block->size;

	void* currentAddress = &block + HEADER_SIZE + block->size;
	block_header* currentBlock = (block_header*)currentAddress;
	while (currentBlock->alloc == 0)
	{
		totalSize += HEADER_SIZE + currentBlock->size;
		currentAddress += HEADER_SIZE + currentBlock->size;
		currentBlock = (block_header*)currentAddress;
	}

	block->size = totalSize;
}
