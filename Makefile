EXEC=mytests

#do not forget to adapt the path to your CUnit directory
all : $(EXEC)

mytests : main.o myalloc.o tests.o
	gcc -o $(EXEC) main.o myalloc.o tests.o -Wall -L$(HOME)/local/lib -lcunit
	
myalloc.o : myalloc.c
	gcc -o myalloc.o -c myalloc.c -Wall

tests.o : tests.c
	gcc -o tests.o -c tests.c -Wall -I$(HOME)/local/include
	
main.o : main.c myalloc.h tests.h
	gcc -c main.c -o main.o -Wall -I$(HOME)/local/include
	
clean :
	rm -rf *.o
	
cleanall : clean
	rm -rf $(EXEC)

start :
	export LD_LIBRARY_PATH=$HOME/local/lib:$LD_LIBRARY_PATH

	
