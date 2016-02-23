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
    std::vector<std::shared_ptr<T>> toRemove;
    std::mutex tasksLock;
    std::atomic_bool clearTasks;
    std::atomic_bool stop;

    O &observer;

public:
    explicit LoadingQueue(O &observer):observer(observer),
                                       clearTasks(false),
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
        withLock([&]{toRemove.push_back(elem);});
    }

    void clear()
    {
        clearTasks = true;
    }

    void performTasks()
    {
        std::shared_ptr<T> current = nullptr;
        stop = false;

        while (!stop)
        {
            withLock([&]
            {
                if (tasksList.empty())
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

            withLock([&]
            {
                if (clearTasks)
                {
                    clearTasks = false;
                    toRemove.clear();
                    tasksList.clear();
                }

                for (const auto &elem : toRemove)
                {
                    tasksList.remove(std::find(tasksList.begin, tasksList.end(), elem));
                }

                toRemove.clear();
            });
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
