#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    void* rsp;
    void* ptr;
    bool done;
} Context;

#define append(arr, item) \
    do { \
        if ((arr)->len >=(arr)->cap) { \
            (arr)->cap = (arr)->cap  == 0? 256: (arr)->cap*2; \
            (arr)->items = realloc((arr)->items, (arr)->cap*sizeof(*(arr)->items)); \
            assert((arr)->items != NULL && "Not enough memory"); \
        } \
        (arr)->items[(arr)->len++] = (item); \
    } while(0)

typedef struct {
    Context* items; 
    size_t len;
    size_t alive;
    size_t cap;
    size_t current;
} Contexts;

Contexts contexts = {0};


void __attribute__((naked)) _restore(void* rsp) {
    asm(
            "movq %rdi, %rsp\n"
            "popq %r15\n"
            "popq %r14\n"
            "popq %r13\n"
            "popq %r12\n"
            "popq %rbx\n"
            "popq %rbp\n"
            "popq %rdi\n"
            "ret\n"
       );
}

void _switch_context(void* rsp) {
    contexts.items[contexts.current].rsp = rsp;
    contexts.current = (contexts.current + 1) % contexts.len;
    while(contexts.items[contexts.current].done) {
        if(contexts.items[contexts.current].ptr != NULL) {
            free(contexts.items[contexts.current].ptr);
            contexts.items[contexts.current].ptr = NULL;
        }
        contexts.current = (contexts.current + 1) % contexts.len;
    }
    _restore(contexts.items[contexts.current].rsp);
}

void __attribute__((naked)) coroutine_yield() {
    asm(
            "pushq %rdi\n"
            "pushq %rbp\n"
            "pushq %rbx\n"
            "pushq %r12\n"
            "pushq %r13\n"
            "pushq %r14\n"
            "pushq %r15\n"
            "movq %rsp, %rdi\n"
            "jmp _switch_context\n"
       );
}

void coroutine_init() {
    append(&contexts, (Context){0});
    contexts.alive=1;
}

void coroutine_done() {
    if(contexts.current == 0) {
        for(int i = 1; i < contexts.len; i++) {
            if(contexts.items[i].ptr != NULL) {
                free(contexts.items[i].ptr);
                contexts.items[i].ptr = NULL;
            }
        }
        free(contexts.items);
        contexts.alive = 0;
        contexts.len = 0;
        return;
    }
    contexts.items[contexts.current].done = true;
    contexts.alive--;
    coroutine_yield();
}

#define STACK_CAP 8*1024

void add(Context context) {
    for(int i = 1; i < contexts.len; i++) { // the main coroutine(id 0, the one in the main function) shouldn't be considered done
        if(contexts.items[i].done) {
            if(contexts.items[i].ptr != NULL) {
                free(contexts.items[i].ptr);
                contexts.items[i].ptr = NULL;
            }
            contexts.items[i] = context;
            return;
        }
    }
    append(&contexts, context);
}

void coroutine_go(void (f)(void*), void* arg) {
    void* ptr = malloc(STACK_CAP);
    void** rsp = ptr + STACK_CAP;
    *(--rsp) = coroutine_done;
    *(--rsp) = f;
    *(--rsp) = arg;
    *(--rsp) = 0;
    *(--rsp) = 0;
    *(--rsp) = 0;
    *(--rsp) = 0;
    *(--rsp) = 0;
    *(--rsp) = 0;

    add((Context){
        .rsp = rsp,
        .ptr = ptr,
    });
    contexts.alive++;
}


size_t coroutine_id() {
    return contexts.current;
}

size_t coroutine_alive() {
    return contexts.alive;
}

