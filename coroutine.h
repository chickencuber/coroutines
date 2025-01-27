#include <stdlib.h>
void coroutine_yield();
void coroutine_init();
void coroutine_done();
void coroutine_go(void (f)(void*), void* arg);
size_t coroutine_id();
size_t coroutine_alive();
