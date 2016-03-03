#include "tests.h"
#include "myalloc.h"

extern int debugLevel;

void inspectMem()
{
	printf("================ Memory inspection ====================\n");
	static int heapLimitAtLaunchInitialized = 0;
	const static void* heapLimitAtLaunch;
	if (!heapLimitAtLaunchInitialized)
	{
		heapLimitAtLaunch = sbrk(0);
		heapLimitAtLaunchInitialized = 1;
	}

	void* currentHeapLimit = sbrk(0);

	if (debugLevel == 1)
	{
		printf("Heap limit at launch :\t%p\n", heapLimitAtLaunch);
		printf("Current heap limit :\t%p\n", currentHeapLimit);
	}

	void* currentTestedBlock = currentHeapLimit;
	block_header* currentTestedHeader;
	size_t countedBytes = 0;
	int stop = 0;
	while (!stop)
	{
		if (currentTestedBlock-BYTES_ALIGNMENT-HEADER_SIZE >= heapLimitAtLaunch)
		{
			currentTestedBlock -= BYTES_ALIGNMENT;
			countedBytes += BYTES_ALIGNMENT;

			currentTestedHeader = (block_header*) (currentTestedBlock-HEADER_SIZE);

			if (currentTestedHeader->zero == 0 && currentTestedHeader->size == countedBytes)
			{
				if (debugLevel == 1)
				{
					printf("-----------------------------------------------------------------\n");
					printf("|                 Block -> size: %d bytes, alloc : %d             |\n", (int) currentTestedHeader->size, (int) currentTestedHeader->alloc);
					printf("|...............................................................|\n");

					int i;
					for (i=0; i<currentTestedHeader->size/4; i++)
						printf("|                     %d bytes                                   |\n", BYTES_ALIGNMENT);
				}

				currentTestedBlock -= HEADER_SIZE;
				countedBytes = 0;
			}
		}
		else
			stop = 1;
	}

	if (debugLevel == 1)
		printf("-----------------------------------------------------------------\n");

	printf("============== End of memory inspection =====================\n");
}
