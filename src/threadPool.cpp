#include "threadPool.h"

void ThreadPool::init(size_t num ){
    for(size_t i = 0;i<num;++i)
    workers.emplace_back(
        std::thread([this]
        {
            for(;;)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock,
                        [this]{ return this->stop || !this->tasks.empty(); });
                    if(this->stop && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        })
    );
}

// the constructor just launches some amount of workers
ThreadPool::ThreadPool()
    :   stop(false)
{
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}
