#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <future>

class ThreadPool
{
public:
    ThreadPool(size_t size = std::thread::hardware_concurrency());
    virtual ~ThreadPool();
    
    template<class Functor, class ...Args>
    std::future<typename std::result_of<Functor(Args...)>::type>
        submit(Functor f, Args ...args);
    
private:
    std::vector<std::thread> threads;
    std::queue<std::packaged_task<void()>> work_queue;
    std::mutex mutex;
    std::atomic_bool done;
    
    ThreadPool(const ThreadPool& other)=delete;
    ThreadPool& operator=(const ThreadPool& other)=delete;
    bool operator==(const ThreadPool& other)const=delete;
    
    void worker_thread();
    void run_pending_task();
    bool q_empty()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if(work_queue.empty()) return true;
        
        return false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

ThreadPool::ThreadPool(size_t size) : done(false)
{
    try
    {
        for(size_t s = 0; s < size; ++s)
            threads.push_back(
                std::thread(&ThreadPool::worker_thread, this));
    }
    catch(...)
    {
        done = true;
        throw;
    }
}

void ThreadPool::worker_thread()
{
    while(!done)
    {
        run_pending_task();
    }
}

template<class Functor, class ...Args>
std::future<typename std::result_of<Functor(Args...)>::type>
    ThreadPool::submit(Functor f, Args ...args)
{
    typedef typename std::result_of<Functor(Args...)>::type return_type;
    std::packaged_task<return_type()> task(std::bind(f, args...));
    std::future<return_type> result = task.get_future();
    
    mutex.lock();
        work_queue.push( std::move( std::packaged_task<void()>( std::move(task) ) ) );
    mutex.unlock();
    
    return result;
}

void ThreadPool::run_pending_task()
{
    if(!q_empty())
    {
        mutex.lock();
            std::packaged_task<void()> task(std::move(work_queue.front()));
            work_queue.pop();
        mutex.unlock();
            
        task();
    }
    else std::this_thread::yield();
}


ThreadPool::~ThreadPool()
{
    done = true;
    
    for(auto &thr : threads)
        if(thr.joinable()) thr.join();
}

#endif // THREAD_POOL_H