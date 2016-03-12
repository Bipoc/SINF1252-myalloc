EXEC=mytests

all : $(EXEC)

mytests : main.o myalloc.o tests.o
	gcc -o $(EXEC) main.o myalloc.o tests.o -Wall
	
myalloc.o : myalloc.c
	gcc -o myalloc.o -c myalloc.c -Wall

tests.o : tests.c
	gcc -o tests.o -c tests.c -Wall -I$(HOME)/local/include
	
main.o : main.c myalloc.h tests.h
	gcc -o main.o -c main.c -Wall -lcunit -L$(HOME)/local/lib -I$(HOME)/local/include
	
clean :
	rm -rf *.o
	
mrproper : clean
	rm -rf $(EXEC)
	
