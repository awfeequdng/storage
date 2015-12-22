#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <thread>
class thread_guard
{
    std::thread &t;
public:
    explicit thread_guard(std::thread &t_):t(t_)
    {}
    ~thread_guard()
    {
        if(t.joinable())
        {
            t.join();
        }
    }
};

class MyThread
{
public:
    MyThread();
};

#endif // MYTHREAD_H
