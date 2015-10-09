fbglTest : fbgl.o fbglMath.o fbglTest.o
	gcc -O3 -o fbglTest fbgl.o fbglMath.o fbglTest.o

fbglMath.o : fbgl.h fbgl.c fbglMath.h fbglMath.c
	gcc -O3 -Wall -c fbglMath.c

fbgl.o : fbgl.h fbgl.c fbglMath.h fbglMath.c
	gcc -O3 -Wall -c fbgl.c

fbglTest.o : fbglTest.c fbgl.h fbgl.c fbglMath.h fbglMath.c
	gcc -O3 -Wall -c fbglTest.c