#pragma once

#include <mutex>

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

    private:
        std::mutex mut{};
    };

    template<class TMutex>
    using LockGuard = std::lock_guard<TMutex>;

} // namespace CE
