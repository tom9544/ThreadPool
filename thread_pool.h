#ifndef _PTHREAD_POOL_
#define _PTHREAD_POOL_

#include "locker.h"
#include <list>
#include <stdio.h>
#include <exception>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <vector>


template<class T>
class threadpool
{
private:
    int thread_number;  //线程池的线程数
    int max_task_number;  //任务队列中的最大任务数
    // pthread_t *all_threads;   //线程数组
    std::vector<pthread_t> all_threads;
    std::list<T *> task_queue; //任务队列
    mutex_locker queue_mutex_locker;  //互斥锁
    sem_locker queue_sem_locker;   //信号量
    bool is_stop; //是否结束线程
public:
    threadpool(int thread_num = 20, int max_task_num = 30);
    ~threadpool();
    bool append_task(T *task);
    void start();
    void stop();
private:
    //线程运行的函数。执行run()函数
    static void *worker(void *arg);
    void run();
};

template <class T>
threadpool<T>::threadpool(int thread_num, int max_task_num):
    thread_number(thread_num), max_task_number(max_task_num),
    is_stop(false)
{
    all_threads.clear();
    if((thread_num <= 0) || max_task_num <= 0) {
        std::cout << "threadpool can't init because thread_number = 0 or max_task_number = 0.\n";
        exit(-1);
    }

    for(size_t i = 0; i < thread_num; ++i) {
        pthread_t _thread;
        all_threads.push_back(_thread);
    }
    if(all_threads.empty()) {
        std::cout << "Can't initial thread pool because thread array can't new.";
        exit(-1);
    }
}

template <class T>
threadpool<T>::~threadpool()
{
    all_threads.clear();
    is_stop = true;
}

template <class T>
void threadpool<T>::stop()
{
    is_stop = true;
    //queue_sem_locker.add();
}

template <class T>
void threadpool<T>::start()
{
    for(int i = 0; i < thread_number; ++i) {
        std::cout << "create the " << i << "th thread.\n";
        if(pthread_create(&(all_threads[i]), NULL, worker, this) != 0) {
            all_threads.clear();
            throw std::exception();
        }
        if(pthread_detach(all_threads[i])) {
            all_threads.clear();
            throw std::exception();
        }
    }
}
//添加任务进入任务队列
template <class T>
bool threadpool<T>::append_task(T *task)
{   //获取互斥锁
    queue_mutex_locker.mutex_lock();
    //判断队列中任务数是否大于最大任务数
    if(task_queue.size() > max_task_number)
    {//是则释放互斥锁
    queue_mutex_locker.mutex_unlock();
    return false;
    }
    //添加进入队列
    task_queue.push_back(task);
    queue_mutex_locker.mutex_unlock();
    //唤醒等待任务的线程
    queue_sem_locker.add();
    return true;
}

template <class T>
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = (threadpool *)arg;
    pool->run();
    return pool;
}

template <class T>
void threadpool<T>::run()
{
    while(!is_stop)
    {   //等待任务
    queue_sem_locker.wait();
    if(errno == EINTR)
    {
        std::cout << "error.\n";
        continue;
    }
    //获取互斥锁
    queue_mutex_locker.mutex_lock();
    //判断任务队列是否为空
    if(task_queue.empty())
    {
        queue_mutex_locker.mutex_unlock();
        continue;
    }
    //获取队头任务并执行
    T *task = task_queue.front();
    task_queue.pop_front();
    queue_mutex_locker.mutex_unlock();
    if(!task) {
        continue;
    }
    std::cout << "pthreadId = " << (unsigned long)pthread_self() << "\n";
    task->doit();  //doit是T对象中的方法
    }
    std::cout << "close " << (unsigned long)pthread_self() << "\n";
}

#endif
