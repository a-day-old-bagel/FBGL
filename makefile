fbglTest : fbgl.o fbglTest.o
	gcc -o fbglTest fbgl.o fbglTest.o

fbgl.o : fbgl.c fbgl.h
	gcc -Wall -c fbgl.c

fbglTest.o : fbglTest.c fbgl.h
	gcc -Wall -c fbglTest.c