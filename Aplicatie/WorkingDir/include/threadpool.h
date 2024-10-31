#ifndef threadpool_H
#define threadpool_H

#include <iostream>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
using namespace std;


class threadpool {
public:
    threadpool(size_t numar_threaduri);
    ~threadpool();
    void enqueue(function<void()> task);
    void stop();

private:
    std::vector<std::thread> workers;
    std::queue<function<void()>>tasks_;
    mutex queue_mutex_;
    condition_variable cv_;
    bool stop_ = false;
};

#endif // threadpool_H