main: main.c coroutine.o coroutine.h 
	gcc -o main main.c coroutine.o
mainc3: mainc3.c3 coroutine.o coroutine.c3
	c3c compile mainc3.c3 coroutine.o coroutine.c3
coroutine.o: coroutine.c
	gcc -c -o coroutine.o coroutine.c
