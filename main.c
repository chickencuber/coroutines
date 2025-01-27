#include <stdio.h>
#include "coroutine.h"

void test(void* arg) {
    long int l = (long int)arg;
    for(int i = 0; i < l; i++) {
        printf("[%lu] %d\n", coroutine_id(), i);
        coroutine_yield();
    }
}

int main() {
    coroutine_init();
    coroutine_go(test, (void*) 10);
    coroutine_go(test, (void*) 10);
    test((void*) 5);
    while(coroutine_alive() > 1) coroutine_yield();
    coroutine_done();
    return 0;
}
