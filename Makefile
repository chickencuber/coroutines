main: main.c coroutine.o coroutine.h
	gcc -o main main.c coroutine.o
coroutine.o: coroutine.c
	gcc -c -o coroutine.o coroutine.c
