#include "myalloc.h"

static void* heapLimitAtLaunch = 0;
static void* actualHeapLimit = 0;

extern int debugLevel;
extern size_t memSize;

void* mymalloc(size_t size)
{
	if (debugLevel == 2)
		printf("\tenter mymalloc with arg : %d\n", size);
	initHeapLimitAtLaunch();

	if (size <=0)
		return NULL;

	if (debugLevel == 2)
		printf("\tmymalloc 2\n");
	
	size_t nbBytes = size/4;
	if (size%4 > 0)
		nbBytes++;
	nbBytes *= 4;

	if (debugLevel == 2)
		printf("\t[[[ mymalloc==> Allocates %d bytes (+ %d header)...\n", (int) nbBytes, HEADER_SIZE);//bytes or bits

	if (debugLevel == 2)
		printf("\tenter Best fit : %d\n", nbBytes);
	void* blockAddress = bestFit(nbBytes);

	if (debugLevel == 2)
		printf("\tpass Best fit\n");

	if (blockAddress == NULL)
		return NULL;

	if (debugLevel == 2)
		printf("\tenter allocateBlock\n");
	allocateBlock(blockAddress, size);

	return (void*)blockAddress + HEADER_SIZE;
}

void* mycalloc(size_t size)
{
	short* address = mymalloc(size);//sizeof short always ==1?
	if (address == NULL)
		return NULL;
	int i;
	for (i=0; i<size; i++)
		*(address+i) = 0;

	return address;
}

void myfree(void* ptr)
{
	if (ptr == NULL)
		return;

	isBlock(ptr);

	block_header* ptrHeader = (block_header*) (ptr-HEADER_SIZE);
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

	block->alloc = 1;
}

void initHeapLimitAtLaunch()
{
	if (debugLevel == 2)
		printf("\tenter initHeapLimitAtLaunch\n");

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

		if (debugLevel>=2)
			printf("%p, %p\n", heapLimitAtLaunch, actualHeapLimit);

		isInit = 1;
	}
}

void* bestFit(size_t nbBytes)
{
	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock;

	if (debugLevel >= 2)
		printf("\tbestFit 1\n");

	void* bestFitAddress=NULL;
	size_t sizeBestFit;

	if (debugLevel >= 2)
		printf("\t\t 2\n");

	while (currentAddress < actualHeapLimit)
	{
		if (debugLevel >= 2)
		printf("\t\t 3\n");
		currentBlock = (block_header*)currentAddress;

		if (debugLevel >= 2)
		printf("\t\t 4\n");

		updateBlock(currentBlock);

		if (debugLevel >= 2)
		printf("\t\t 5\n");

		if (currentBlock->alloc == 0)
		{
			if (currentBlock->size >= nbBytes && (bestFitAddress == NULL || currentBlock->size < sizeBestFit))//add shortcut if size==nbBytes? break program behavior but increase speed
			{
				bestFitAddress = currentAddress;
				sizeBestFit = currentBlock->size;
			}
		}

		if (debugLevel >= 2)
		printf("\t\t 6\n");
		currentAddress += currentBlock->size + HEADER_SIZE;
	}

	if (debugLevel >= 2)
		printf("\t\t 7\n");

	return bestFitAddress;
}

void updateBlock(block_header* block)
{
	if (debugLevel >= 2)
		printf("\t\t enter updateblock\n");

	if (block == NULL || block->alloc == 1)
		return;

	if (debugLevel >= 2)
		printf("\t\t\t 1\n");

	size_t totalSize = block->size;

	if (debugLevel >= 2)
		printf("\t\t\t 2\n");

	void* currentAddress = (void*)block + HEADER_SIZE + block->size;
	block_header* currentBlock = (block_header*)currentAddress;
	if (debugLevel >= 2)
		printf("\t\t\t 3\n");

	while (currentBlock->alloc == 0 && currentAddress < actualHeapLimit)
	{
		if (debugLevel >= 2)
		printf("\t\t\t 4\n");
		totalSize += HEADER_SIZE + currentBlock->size;
		if (debugLevel >= 2)
		printf("\t\t\t 4a\n");
		currentAddress += HEADER_SIZE + currentBlock->size;
		if (debugLevel >= 2)
		printf("\t\t\t 4b\n");
		currentBlock = (block_header*)currentAddress;
		if (debugLevel >= 2)
		printf("\t\t\t 4c\n");
	}

	if (debugLevel >= 2)
		printf("\t\t\t 6\n");

	block->size = totalSize;
}
