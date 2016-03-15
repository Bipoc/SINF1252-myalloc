#include <string.h>

#include "myalloc.h"
#include "tests.h"

size_t memSize;

int main(int argc, char *argv[])
{
	// cunitTests don't need you to set memsize, memsize is set to be 40 in cunitTests
	if (argc>=2)
	{
		memSize = (size_t)strtol(argv[1], NULL, 10);
		printf("HeapMemory asked is : %ld\n", memSize);
	}//note don't do a call to mymalloc or mycalloc before cunitTests, if you use cunitTests

	//cunitTests();//decomment to launch

	return EXIT_SUCCESS;
}
