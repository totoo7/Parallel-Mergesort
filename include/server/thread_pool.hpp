#ifndef THREAD_POOL
#define THREAD_POOL

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
    public:
        ThreadPool(size_t num_threads);

        template<typename T> 
        auto enqueue(T task) -> std::future<decltype(task())>;
        size_t size() const { return workers.size(); }
        ~ThreadPool();
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop = false;
};

template<typename T>
auto ThreadPool::enqueue(T task) -> std::future<decltype(task())> {
    using ReturnType = decltype(task());

    auto wrapper = std::make_shared<std::packaged_task<ReturnType()>>(std::move(task));

    std::future<ReturnType> result = wrapper->get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace([wrapper]() {
            (*wrapper)();
        });
    }

    condition.notify_one();
    return result;
}


#endif