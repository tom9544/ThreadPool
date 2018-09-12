#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include "thread_pool.h"

class task
{
private:
    int number;

public:
    task(int num) : number(num)
    {
    }
    ~task()
    {
    }

    void doit()
    {
        std::cout << "this is the " << number << "th task.\n";
    }
};

int main() {
    task *ta;
    threadpool<task> pool(10, 15);
    for(int i = 0; i < 20; ++i) {
        ta = new task(i);
        pool.append_task(ta);
    }
    pool.start();
    sleep(2);
    printf("close the thread pool\n");
    pool.stop();
    return 0;
}
