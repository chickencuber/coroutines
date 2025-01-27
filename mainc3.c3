import std::io;
import coroutine;

fn void test(void* arg) {
    usz l = (usz) arg;
    for(int i = 0; i < l; i++) {
        io::printfn("[%d] %d", coroutine::id(), i);
        coroutine::yield();
    }
}

fn void main() {
   coroutine::init();
   coroutine::go(&test, (void*) 10);
   coroutine::go(&test, (void*) 10);
   test((void*) 5);
   while(coroutine::alive() > 1) coroutine::yield();
   coroutine::done();
}
