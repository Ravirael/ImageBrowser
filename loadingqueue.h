#ifndef LOADINGQUEUE_H
#define LOADINGQUEUE_H

#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <condition_variable>
#include "asyncpixmaploader.h"

template <typename T, typename O>
class LoadingQueue
{
    std::list<T> tasksList;
    std::mutex tasksLock;
    std::condition_variable cv;

    O observer;
    std::atomic_bool stop;


public:
    explicit LoadingQueue(O observer):observer(observer),
                                       stop(false)
    {

    }

    void addToBegining(T elem)
    {
        std::unique_lock<std::mutex> lock(tasksLock);
        tasksList.push_back(elem);
        cv.notify_one();
    }

    void addToEnd(T elem)
    {
        std::unique_lock<std::mutex> lock(tasksLock);
        tasksList.push_front(elem);
        cv.notify_one();
    }

    void remove(T elem)
    {
        withLock([&]{tasksList.remove(elem);});
    }

    void clear()
    {
        withLock([&]{tasksList.clear();});
    }

    void stopTasks()
    {
        stop = true;
        cv.notify_one();
    }

    void performTasks()
    {
        T current = nullptr;
        stop = false;

        while (!stop)
        {
            withLock([&]
            {
                if (!tasksList.empty())
                {
                    current = tasksList.back();
                    tasksList.pop_back();
                }
                else
                {
                    current = nullptr;
                }
            });

            if (current)
            {
                (*current)();
                observer(current);
            }
            else
            {
                std::unique_lock<std::mutex> lock(tasksLock);
                while (tasksList.empty() && !stop)
                {
                    cv.wait(lock);
                }
            }

        }
    }

private:
    template <typename U>
    void withLock(U lambda)
    {
        tasksLock.lock();
        lambda();
        tasksLock.unlock();
    }


    //void add(const AsyncPixmapLoader &loader);
};

#endif // LOADINGQUEUE_H
