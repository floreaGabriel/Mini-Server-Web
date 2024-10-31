#include "threadpool.h"






threadpool::threadpool(size_t numar_threaduri) 
{
    for (size_t i=0;i<numar_threaduri;++i)
    {
        workers.emplace_back([this] {
            while (true)
            {
                /* code */
                function<void()> task;
                {
                    unique_lock<mutex> lock(queue_mutex_);
                    cv_.wait(lock,[this] {
                        return !tasks_.empty() || stop_;
                    });

                    if(stop_ && tasks_.empty())
                    {
                        return;
                    }

                    task = move(tasks_.front());
                    tasks_.pop();
                }

                task();
            }
            
        });
    }
}

threadpool::~threadpool() {
    stop();

}

void threadpool::stop() {
    {
        unique_lock<mutex> lock(queue_mutex_);
        stop_ = true;
    }

    cv_.notify_all();

    for(auto& thread : workers){
        thread.join();
    }
}


void threadpool::enqueue(function<void()> task)
{
    {
        unique_lock<std::mutex> lock(queue_mutex_);
        tasks_.emplace(move(task));
    }
    cv_.notify_one();
}


