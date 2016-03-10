#include "myalloc.h"

static void* heapLimitAtLaunch = 0;
extern int debugLevel;

void* mymalloc(size_t size)
{
	initHeapLimitAtLaunch();//setup new rules

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

void myfree(void* ptr)//securised way? table with address?
{
	if (ptr == NULL)
		return;

	block_header* ptrHeader = (block_header*) (ptr-HEADER_SIZE);
	ptrHeader->alloc = 0;
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

void initHeapLimitAtLaunch()//setup new rules
{
	static int isInit = 0;
	if (!isInit)
	{
		heapLimitAtLaunch = sbrk(0);
		isInit = 1;
	}
}

void* bestFit(size_t nbBytes)
{
	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock;

	void* heapLimit = sbrk(0);
	if ((long)heapLimit == -1)
		return (void*)-1;//set an error like this? exit(EXIT_FAILURE)

	void* bestFitAddress=NULL;
	size_t sizeBestFit;

	while (currentAddress < heapLimit)
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

	if (bestFitAddress != &currentBlock)
		decreaseHeapLimit(currentBlock);

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

void decreaseHeapLimit(block_header* lastBlock)
{
	if (lastBlock==NULL || lastBlock->alloc==1)
		return;

	sbrk( - (HEADER_SIZE + lastBlock->size) );//test if it works useful or not?
}
