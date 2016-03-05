#include "myalloc.h"

static void* heapLimitAtLaunch = 0;
extern int debugLevel;

void* mymalloc(size_t size)
{
	initHeapLimitAtLaunch();

	if (size > 0)
	{
		size_t nbBytes = size/4;
		if (size%4 > 0)
			nbBytes++;
		nbBytes *= 4;

		if (debugLevel == 2)
			printf("\t[[[ mymalloc==> Allocates %d bytes (+ %d header)...\n", (int) nbBytes, HEADER_SIZE);

		void* beginningBlock = sbrk(nbBytes+HEADER_SIZE);
		if ((long) beginningBlock == -1)
			return NULL;

		block_header* ptrHeader = (block_header*) beginningBlock;
		ptrHeader->size = nbBytes ;
		ptrHeader->zero = 0;
		ptrHeader->alloc = 1;

		if (debugLevel == 2)
			printf("\tAllocation of %d bytes complete (ptr : %p -> header : %p) ]]]\n", (int) nbBytes, beginningBlock+HEADER_SIZE, beginningBlock);
		else if (debugLevel == 1)
			printf("\t[mymalloc : %d(+%d) bytes (block : %p)]\n", (int) nbBytes, HEADER_SIZE, beginningBlock);

		return beginningBlock+HEADER_SIZE;
	}
	return NULL;
}

void myfree(void* ptr)
{
	if (ptr != NULL)
	{
		block_header* ptrHeader = (block_header*) (ptr-HEADER_SIZE);
		size_t bytesToFree = ptrHeader->size;
		if (debugLevel == 2)
			printf("\t[[[ myfree==> Freeing %d bytes (ptr : %p -> header %p)...\n", (int) bytesToFree, ptr, ptrHeader);
		else if (debugLevel == 1)
			printf("\t[myfree : %d(+%d) bytes (block : %p)\n", (int) bytesToFree, HEADER_SIZE, ptrHeader);
		ptrHeader->alloc = 0;

		void* endHeap = sbrk(0);
		if ((long) endHeap != -1)
		{
			if (debugLevel == 2)
				printf("\tNext block : %p\tendHeap %p\n", ptr+ptrHeader->size, endHeap);
			if (ptr+ptrHeader->size == endHeap)
			{
				if (debugLevel == 2)
					printf("\tShifting heap limit...\n");
				if (bytesToFree > 0)
					sbrk(-bytesToFree-HEADER_SIZE);
				else
					sbrk(-HEADER_SIZE);

				dropHeapLimit((void*) ptrHeader);
			}
		}

		if (debugLevel == 2)
			printf("\tFreed %d bytes ]]]\n", (int) bytesToFree);
		else if (debugLevel == 1)
			printf("\tmyfree done]\n");
	}
}

void dropHeapLimit(void* ptrLastFreedBlock)
{
	void* currentTestedBlock = ptrLastFreedBlock;
	block_header* currentTestedHeader;
	size_t bytesCounted;
	int stop = 0;
	while (!stop)
	{
		if (currentTestedBlock-BYTES_ALIGNMENT-HEADER_SIZE >= heapLimitAtLaunch)
		{
			currentTestedBlock -= BYTES_ALIGNMENT;
			bytesCounted += BYTES_ALIGNMENT;
			currentTestedHeader = (block_header*) (currentTestedBlock-HEADER_SIZE);

			if (debugLevel == 1)
				printf("\tTesting previous header -> zero:%d, size:%d, alloc:%d\n", (int) currentTestedHeader->zero, currentTestedHeader->size, currentTestedHeader->alloc);

			if (currentTestedHeader->zero == 0 && currentTestedHeader->size == bytesCounted)
			{
				if (currentTestedHeader->alloc == 0)
				{
					if (debugLevel == 2)
						printf("\tFreeing another %d free bytes (+ %d for header) at address : %p (-> header %p)...\n", (int) bytesCounted, HEADER_SIZE, currentTestedBlock, (void*) currentTestedHeader);
					else if (debugLevel == 1)
						printf("\tFreeing another %d(+%d) bytes (block : %p)\n", (int) bytesCounted, HEADER_SIZE, (void*) currentTestedHeader);
					sbrk(-bytesCounted-HEADER_SIZE);

					bytesCounted = 0;
					currentTestedBlock -= HEADER_SIZE;
				}
				else
					stop = 1;
			}
		}
		else
			stop = 1;
	}
}

void initHeapLimitAtLaunch()
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
	if ((int)heapLimit == -1)
		return -1;//set an error like this? exit(EXIT_FAILURE)

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
