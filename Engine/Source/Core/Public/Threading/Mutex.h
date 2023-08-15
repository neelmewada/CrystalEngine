#pragma once

#include <mutex>
#include <shared_mutex>
#include <semaphore>
#include <chrono>

namespace CE
{
    
    class CORE_API Mutex
    {
    public:
        Mutex();
        ~Mutex();

        CE_INLINE void Lock() { mut.lock(); }
        CE_INLINE void Unlock() { mut.unlock(); }
        CE_INLINE bool TryLock() { return mut.try_lock(); }

        void lock() { mut.lock(); }
        void unlock() { mut.unlock(); }
        bool try_lock() { return mut.try_lock(); }

		inline operator std::mutex&()
		{
			return mut;
		}

    private:
        std::mutex mut{};
    };

	class CORE_API SharedMutex
	{
	public:
		SharedMutex();
		~SharedMutex();

		CE_INLINE void Lock() { mut.lock(); }
		CE_INLINE void Unlock() { mut.unlock(); }
		CE_INLINE bool TryLock() { return mut.try_lock(); }

		void lock() { mut.lock(); }
		void unlock() { mut.unlock(); }
		bool try_lock() { return mut.try_lock(); }

		inline operator std::shared_mutex& ()
		{
			return mut;
		}

	private:
		std::shared_mutex mut{};
	};

	class CORE_API RecursiveMutex
	{
	public:
		RecursiveMutex() {}
		~RecursiveMutex() {}

		CE_INLINE void Lock() { mut.lock(); }
		CE_INLINE void Unlock() { mut.unlock(); }
		CE_INLINE bool TryLock() { return mut.try_lock(); }

		void lock() { mut.lock(); }
		void unlock() { mut.unlock(); }
		bool try_lock() { return mut.try_lock(); }

		inline operator std::recursive_mutex& ()
		{
			return mut;
		}

	private:
		std::recursive_mutex mut{};
	};
    
    template<class TMutex>
    struct LockGuard
    {
    public:
        LockGuard(TMutex& mutex) : mutex(mutex)
        {
            mutex.Lock();
        }

        ~LockGuard() noexcept
        {
            mutex.Unlock();
        }

        LockGuard(const LockGuard& copy) = delete;
        LockGuard operator=(const LockGuard& copy) = delete;

    private:
		TMutex& mutex;
    };

	template<typename T>
	using Atomic = std::atomic<T>;

	using BinarySemaphore = std::binary_semaphore;

	using CountingSemaphore = std::counting_semaphore<>;

} // namespace CE
