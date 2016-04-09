#include "Executor.h"

//#include <algorithm>
#include <string>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "executor");

Executor::Executor(size_t nThreads)
    : nThreads(nThreads)
{
    nThreads = std::max((size_t)4, nThreads);
    taskQueues.resize(nThreads);
    for (size_t i = 0; i < nThreads; i++)
        thread_pool.emplace_back([this, i] { run(i); });
}

Executor::~Executor()
{
    stop();
    for (auto &t : thread_pool)
        t.join();
}

void Executor::run(size_t i)
{
    while (!done)
    {
        std::string error;
        try
        {
                Task t;
                const size_t spin_count = nThreads * 4;
                for (auto n = 0; n != spin_count; ++n)
                {
                    if (taskQueues[(i + n) % nThreads].try_pop(t))
                        break;
                }
                // no task popped, probably shutdown command was issues
                if (!t && !taskQueues[i].pop(t))
                    break;
                t();
        }
        catch (const std::exception &e)
        {
            error = e.what();
        }
        catch (...)
        {
            error = "unknown exception";
        }
        if (!error.empty())
        {
            LOG_ERROR(logger, "executor: " << this << ", thread #" << i + 1 << ", error: " << error);
        }
    }
}

Executor &getTaskExecutor()
{
    static Executor executor;
    return executor;
}
