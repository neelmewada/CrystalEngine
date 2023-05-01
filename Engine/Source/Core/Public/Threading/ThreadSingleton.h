#pragma once

#include "Mutex.h"
#include "Thread.h"

namespace CE
{
    
    template<class T>
    class ThreadSingleton
    {
    protected:
        ThreadSingleton() : threadId(Thread::GetCurrentThreadId())
        {}

    public:

        ThreadSingleton(const ThreadSingleton& copy) = delete;
        ThreadSingleton operator=(const ThreadSingleton& copy) = delete;

        FORCE_INLINE static T& Get()
        {
            LockGuard<Mutex> lock{ mutex };
            auto currentThreadId = Thread::GetCurrentThreadId();

            if (!map.KeyExists(currentThreadId))
                map.Add({ currentThreadId, new T });

            return *map[currentThreadId];
        }

        ThreadId GetThreadId() const
        {
            return threadId;
        }

        static void DestroyAll()
        {
            LockGuard<Mutex> lock{ mutex };
            for (auto [threadId, instance] : map)
            {
                delete instance;
            }
            map.Clear();
        }

        static HashMap<ThreadId, T*>& GetMap()
        {
            return map;
        }

    private:

        ThreadId threadId;

        static Mutex mutex;
        static HashMap<ThreadId, T*> map;
    };

    template<typename T> Mutex ThreadSingleton<T>::mutex{};
    template<typename T> HashMap<ThreadId, T*> ThreadSingleton<T>::map{};

} // namespace CE
