#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include "thread_pool.h"

class task
{
private:
    int number;

public:
    task(int num):
        number(num)
    {}
    ~task() {}

    void doit() {
        std::cout << "this is the " << number << "th task.\n";
    }
};

int main() {
    threadpool<task> pool(10, 15);
    pool.start();

    task *ta;
    for(int i = 0; i < 20; ++i) {
        ta = new task(i);
        pool.append_task(ta);
    }

    for(auto &pool_thread: pool.all_threads) {
        void *tret;
        int err = pthread_join(pool_thread, &tret);
        if(0 != err) {
            std::cout << "Error join.\n";
            exit(-1);
        }
        std::cout << "Return " << (long)tret << "\n";
    }


    sleep(1);
    std::cout << "Close the thread pool.\n";
    // pool.stop();
    return 0;
}
