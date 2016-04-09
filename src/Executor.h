#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using Task = std::function<void()>;

class TaskQueue
{
    using Tasks = std::deque<Task>;
    using Lock = std::unique_lock<std::mutex>;

public:
    TaskQueue() {}
    TaskQueue(TaskQueue &&rhs)
    {
        Lock lock(rhs.m);
        q = std::move(rhs.q);
    }

    bool try_push(Task &t)
    {
        {
            Lock lock(m, std::try_to_lock);
            if (!lock)
                return false;
            q.emplace_back(std::move(t));
        }
        cv.notify_one();
        return true;
    }

    bool try_pop(Task &t)
    {
        Lock lock(m, std::try_to_lock);
        if (!lock || q.empty() || done_)
            return false;
        t = std::move(q.front());
        q.pop_front();
        return true;
    }

    void push(Task &t)
    {
        {
            Lock lock(m);
            q.emplace_back(std::move(t));
        }
        cv.notify_one();
    }

    bool pop(Task &t)
    {
        Lock lock(m);
        while (q.empty() && !done_)
            cv.wait(lock);
        if (q.empty() || done_)
            return false;
        t = std::move(q.front());
        q.pop_front();
        return true;
    }

    void done()
    {
        {
            Lock lock(m);
            done_ = true;
        }
        cv.notify_all();
    }

private:
    Tasks q;
    std::mutex m;
    std::condition_variable cv;
    bool done_ = false;
};

class Executor
{
    using Threads = std::vector<std::thread>;
    using TaskQueues = std::vector<TaskQueue>;

public:
    Executor(size_t nThreads = std::thread::hardware_concurrency());
    ~Executor();

    size_t numberOfThreads() const { return nThreads; }

    template <class T>
    void push(T &&t)
    {
        static_assert(!std::is_lvalue_reference<T>::value, "Argument cannot be an lvalue");

        Task task([t = std::move(t)]{ t(); });
        auto i = index++;
        for (auto n = 0; n != nThreads; ++n)
        {
            if (taskQueues[(i + n) % nThreads].try_push(task))
                return;
        }
        taskQueues[i % nThreads].push(task);
    }

    void stop()
    {
        done = true;
        for (auto &q : taskQueues)
            q.done();
    }

private:
    Threads thread_pool;
    TaskQueues taskQueues;
    size_t nThreads;
    std::atomic_size_t index;
    bool done = false;

    void run(size_t i);
};

Executor &getTaskExecutor();
