#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "myalloc.h"
#include "tests.h"

int debugLevel = 0;
size_t memSize;

int main(int argc, char *argv[])
{
	if (argc<1)
		exit(EXIT_FAILURE);

	memSize = (size_t)strtol(argv[1], NULL, 10);

	//printf("%d\n", memSize);


	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	
	CU_pSuite pSuite = NULL;	

	pSuite = CU_add_suite("my_suite", initTests , finishTests);
	if (NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (NULL == CU_add_test(pSuite, "Test memory as good size", checkSize))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_run_tests();
	CU_basic_show_failures(CU_get_failure_list());

	CU_cleanup_registry();

	return EXIT_SUCCESS;
}
