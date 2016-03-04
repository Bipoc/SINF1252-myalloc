#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "myalloc.h"
#include "tests.h"

int debugLevel = 1;//0 - 2

int main(int argc, char *argv[])
{
	void* currentHeapLimit = sbrk(0);
	printf("Current heap limit : %p\n", currentHeapLimit);
	inspectMem();

	//malloc(int)
	int* ptrInt = (int*) mymalloc(sizeof(int));

	*ptrInt = 1252;

	inspectMem();
	myfree(ptrInt);

	inspectMem();
/*
	//malloc(long)
	printf("\nTries to allocate %d bytes (long)\n", (int) sizeof(long));
	long* ptrLong = (long*) mymalloc(sizeof(long));
	if (ptrLong != NULL)
		printf("Well allocated\n");

	*ptrLong = 900000000010101000;
	printf("Value stored : %ld\n", *ptrLong);

	currentHeader = (block_header*) ((void*) ptrLong-HEADER_SIZE);
	printf("size : %d, zero : %d, alloc : %d\n", currentHeader->size, currentHeader->zero, currentHeader->alloc);

	myfree(ptrLong);

	//malloc(short)
	printf("\nTries to allocate %d bytes (short)\n", (int) sizeof(short));
	short* ptrShort = (short*) mymalloc(sizeof(short));
	if (ptrShort != NULL)
		printf("Well allocated\n");

	*ptrShort = 123;
	printf("Value stored : %d\n", *ptrShort);

	currentHeader = (block_header*) ((void*) ptrShort-HEADER_SIZE);
	printf("size : %d, zero : %d, alloc : %d\n", currentHeader->size, currentHeader->zero, currentHeader->alloc);

	myfree(ptrShort);

	//malloc(char[5])
	printf("\nTries to allocate %d bytes (char[5])\n", (int) sizeof(char)*5);
	char* tabChar = mymalloc(sizeof(char)*5);
	if (tabChar != NULL)
		printf("Well allocated\n");

	int i;
	for (i=0; i<5; i++)
	{
		tabChar[i] = i*2;
		printf("Values stored : %d\n", tabChar[i]);
	}

	currentHeader = (block_header*) ((void*) tabChar-HEADER_SIZE);
	printf("size : %d, zero : %d, alloc : %d\n", currentHeader->size, currentHeader->zero, currentHeader->alloc);

	myfree(tabChar);
*/
	//not order free
	//malloc(short)
	short* ptr1 = (short*) mymalloc(sizeof(short));
	*ptr1 = 333;

	//malloc(char)
	char* ptr2 = (char*) mymalloc(sizeof(char));
	*ptr2 = 27;

	inspectMem();

	myfree(ptr1);

	inspectMem();

	printf("Last value stored : %d (%p)\n", *ptr2, ptr2);

	myfree(ptr2);

	inspectMem();
	currentHeapLimit = sbrk(0);
	printf("Current heap limit : %p\n", currentHeapLimit);
	
	return EXIT_SUCCESS;
}