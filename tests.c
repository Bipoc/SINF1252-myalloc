#include "tests.h"
#include "myalloc.h"

static void* heapLimitAtLaunch;
extern size_t memSize;

void setHeapLimitAtLaunch()
{
	heapLimitAtLaunch = sbrk(0);
}

int cunitTests()
{
	setHeapLimitAtLaunch();

	memSize = MEM_SIZE_TESTS;

	mymalloc(0);


	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	

	CU_pSuite pSuiteBasic = NULL;
	pSuiteBasic = CU_add_suite("limits_suite", initBasicTests, finishTests);
	if (NULL == pSuiteBasic)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(pSuiteBasic, "Test basic free", testBasicFree)
		|| NULL == CU_add_test(pSuiteBasic, "Test some allocation and deallocation", testBasicAllocAndFree)
		|| NULL == CU_add_test(pSuiteBasic, "Test memory block by block", testBlockbyBlockBasic)
		|| NULL == CU_add_test(pSuiteBasic, "Test call must fail", mustFail))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}


	CU_pSuite pSuiteFree = NULL;
	pSuiteFree = CU_add_suite("limits_suite", initFreeTests, finishTests);
	if (NULL == pSuiteFree)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(pSuiteFree, "Test free at start, center and end of memory", testBlockbyBlockFree)
		|| NULL == CU_add_test(pSuiteFree, "Test regroup of free blocks", allocMustSuccess))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}


	CU_pSuite pSuiteLimits = NULL;
	pSuiteLimits = CU_add_suite("limits_suite", initLimitsTests, finishTests);
	if (NULL == pSuiteLimits)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(pSuiteLimits, "Test memory block by block", testBlockbyBlockLimits)
		|| NULL == CU_add_test(pSuiteLimits, "Test allocation without memory", allocMustFail))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}


	CU_pSuite pSuiteCalloc = NULL;	
	pSuiteCalloc = CU_add_suite("calloc_suite", initCallocTests , finishTests);
	if (NULL == pSuiteCalloc)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(pSuiteCalloc, "Test memory as good size", checkSize)
		|| NULL == CU_add_test(pSuiteCalloc, "Test memory is well set at zero by calloc", checkCalloc))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}


	CU_basic_run_tests();
	CU_basic_show_failures(CU_get_failure_list());

	CU_cleanup_registry();

	printf("\n");
	return 0;
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

void testBasicFree()
{
	block_header* block =(block_header*)heapLimitAtLaunch;

	CU_ASSERT(block->alloc == 0);
}

void testBasicAllocAndFree()
{
	void* p1; void* p2;
	mymalloc(8); p1=mymalloc(1); p2=mymalloc(5); mycalloc(1, 1);
	myfree(p1); myfree(p2);
	p1=mymalloc(3);

	CU_ASSERT(p1 != NULL);
}

void testBlockbyBlockBasic()
{
	int result = 1;

	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock = (block_header*)currentAddress;

	result = result && (currentBlock->alloc == 1) && (currentBlock->size == 8);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 1) && (currentBlock->size == 4);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 0) && (currentBlock->size == 8);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 1) && (currentBlock->size == 4);

	currentAddress += HEADER_SIZE + currentBlock->size;
	result = result && (currentAddress == heapLimitAtLaunch + memSize);

	CU_ASSERT(result);
}

void mustFail()
{
	void *ptr1 = mymalloc(9);
	CU_ASSERT(ptr1 == NULL)

	ptr1=mycalloc(3, 3);
	CU_ASSERT(ptr1 == NULL);	
}

void testBlockbyBlockFree()
{
	int result = 1;

	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock = (block_header*)currentAddress;

	result = result && (currentBlock->alloc == 0) && (currentBlock->size == 4);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 0) && (currentBlock->size == 8);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 1) && (currentBlock->size == 4);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 0) && (currentBlock->size == 4);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 0) && (currentBlock->size == 0);

	currentAddress += HEADER_SIZE + currentBlock->size;
	result = result && (currentAddress == heapLimitAtLaunch + memSize);

	CU_ASSERT(result);
}

void allocMustSuccess()
{
	void *ptr1 = mymalloc(16);
	CU_ASSERT(ptr1 != NULL);

	void *ptr2 = mymalloc(8);
	CU_ASSERT(ptr2 != NULL);
}

void testBlockbyBlockLimits()
{
	int result = 1;

	void* currentAddress = heapLimitAtLaunch;
	block_header* currentBlock = (block_header*)currentAddress;

	result = result && (currentBlock->alloc == 1) && (currentBlock->size == 4);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 0) && (currentBlock->size == 0);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 1) && (currentBlock->size == 8);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 1) && (currentBlock->size == 8);

	currentAddress += HEADER_SIZE + currentBlock->size;
	currentBlock = (block_header*)currentAddress;
	result = result && (currentBlock->alloc == 0) && (currentBlock->size == 0);

	currentAddress += HEADER_SIZE + currentBlock->size;
	result = result && (currentAddress == heapLimitAtLaunch + memSize);

	CU_ASSERT(result);
}

void allocMustFail()
{
	void *ptr = mymalloc(1);

	CU_ASSERT(ptr == NULL);
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
	int zero = 1;

	while (currentAddress < heapLimitAtLaunch + memSize)
	{

		block_header* currentBlock = (block_header*)currentAddress;	
		zero = zero && isFullOfZeros(currentAddress + HEADER_SIZE, currentBlock->size);

		currentAddress += HEADER_SIZE + currentBlock->size;
	}

	CU_ASSERT(zero);
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

int initBasicTests()
{
	fullBlockWithOnes(heapLimitAtLaunch);

	void *p1;

	p1=mymalloc(36);
	myfree(p1);

	return 0;
}

int initFreeTests()
{
	fullBlockWithOnes(heapLimitAtLaunch);

	void* p1=mymalloc(4); void* p2=mymalloc(8); mymalloc(4); void* p3=mymalloc(4);
	myfree(p1); myfree(p2); myfree(p3);

	return 0;
}

int initLimitsTests()
{
	fullBlockWithOnes(heapLimitAtLaunch);

	void* p1=mymalloc(8); mymalloc(8); void* p2=mymalloc(12);
	myfree(p1); mymalloc(1);
	myfree(p2); mymalloc(7);

	return 0;
}

int initCallocTests()
{
	fullBlockWithOnes(heapLimitAtLaunch);

	mycalloc(2, 2); void* p2=mycalloc(2, 3); mycalloc(2, 3); mycalloc(4, 2);
	myfree(p2);

	mycalloc(1, 3); mycalloc(1, 0);

	return 0;
}

int finishTests()
{
	resetMem();

	return 0;
}
