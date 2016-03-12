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

	void* blockAddress = bestFit(nbBytes);


	if (blockAddress == NULL)
		return NULL;

	allocateBlock(blockAddress, nbBytes);

	return (void*)blockAddress + HEADER_SIZE;
}

void* mycalloc(size_t size)
{
	size_t nbBytes = size/4;
	if (size%4 > 0)
		nbBytes++;
	nbBytes *= 4;
	
	char* address = (char*)mymalloc(nbBytes);//sizeof char always ==1?
	if (address == NULL)
		return NULL;
	int i;
	for (i=0; i<nbBytes; i++)
		*(address+i) = 0;

	return address;
}

void myfree(void* ptr)
{
	if (ptr == NULL)
		return;

	ptr -= HEADER_SIZE;

	isBlock(ptr);
	
	block_header* ptrHeader = (block_header*)ptr;
	ptrHeader->alloc = 0;
}

void isBlock(void* ptr)
{
	if (ptr == NULL)
		exit(EXIT_FAILURE);//TODO change?

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

	block->size = size;
	block->alloc = 1;}

void initHeapLimitAtLaunch()
{
	static int isInit = 0;
	if (!isInit)
	{
		heapLimitAtLaunch = sbrk(0);

		size_t temporary = memSize/4;
		if (memSize%4 > 0)
			temporary++;
		memSize = temporary * 4;

		void *answer = sbrk(memSize);
		if ((long)answer == -1)
			exit(EXIT_FAILURE);//TODO add message with errno

		actualHeapLimit = heapLimitAtLaunch + memSize;

		block_header* firstBlock = (block_header*)heapLimitAtLaunch;
		firstBlock->alloc = 0;
		firstBlock->zero = 0;
		firstBlock->size = memSize - HEADER_SIZE;

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

	void* currentAddress = (void*)block + HEADER_SIZE + block->size;
	block_header* currentBlock = (block_header*)currentAddress;

	while (currentBlock->alloc == 0 && currentAddress < actualHeapLimit)
	{
		totalSize += HEADER_SIZE + currentBlock->size;

		currentAddress += HEADER_SIZE + currentBlock->size;
		currentBlock = (block_header*)currentAddress;
	}

	block->size = totalSize;
}
