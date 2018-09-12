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
    printf("this is the %dth task\n", number);
    }
};

int main()
{
    task *ta;
    threadpool<task> pool(10, 15);
//    pool.start();
    for(int i = 0; i < 20; ++i)
    {
    ta = new task(i);
//  sleep(2);
    pool.append_task(ta);
    }
    pool.start();
    sleep(10);
    printf("close the thread pool\n");
    pool.stop();
    pause();
    return 0;
}
