#ifndef LOADINGQUEUE_H
#define LOADINGQUEUE_H

#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include "asyncpixmaploader.h"

template <typename T, typename O>
class LoadingQueue
{
    std::list<std::shared_ptr<T>> tasksList;
    std::mutex tasksLock;

    O &observer;
    std::atomic_bool stop;


public:
    explicit LoadingQueue(O &observer):observer(observer),
                                       stop(false)
    {

    }

public:
    void addToBegining(const std::shared_ptr<T> &elem)
    {
        withLock([&]{tasksList.push_back(elem);});
    }

    void addToEnd(const std::shared_ptr<T> &elem)
    {
        withLock([&]{tasksList.push_front(elem);});
    }

    void remove(const std::shared_ptr<T> &elem)
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
    }

    void performTasks()
    {
        std::shared_ptr<T> current = nullptr;
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
                std::this_thread::yield();
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
