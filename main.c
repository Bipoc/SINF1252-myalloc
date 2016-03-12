#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "myalloc.h"
#include "tests.h"

void CU_basic_run_tests();
void CU_basic_show_failures();

int debugLevel = 0;
size_t memSize;

int main(int argc, char *argv[])
{
	/**
	if (argc<1)
		exit(EXIT_FAILURE);

	memSize = (size_t)strtol(argv[1], NULL, 10);
	
	printf("%d\n", memSize);
	**/


	setHeapLimitAtLaunch();

	memSize = MEM_SIZE_TESTS;

	mymalloc(0);


	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	


	//other tests suites 




	CU_pSuite pSuiteCalloc = NULL;	

	pSuiteCalloc = CU_add_suite("calloc_suite", initCallocTests , finishCallocTests);
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

	return EXIT_SUCCESS;
}
