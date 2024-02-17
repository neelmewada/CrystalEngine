
#include "CoreMinimal.h"

namespace CE
{

	Mutex::Mutex()
	{
		
	}

	Mutex::~Mutex()
	{
	}

    SharedMutex::SharedMutex()
    {
    }

    SharedMutex::~SharedMutex()
    {
    }

    void SharedRecursiveMutex::Lock()
    {
        std::unique_lock sync_lock(m_mtx);
        m_cond_var.wait(sync_lock, [this] { return can_exclusively_lock(); });
        if (is_exclusive_locked_on_this_thread())
        {
            increment_exclusive_lock();
        }
        else
        {
            start_exclusive_lock();
        }
    }

    bool SharedRecursiveMutex::TryLock()
    {
        std::unique_lock sync_lock(m_mtx);
        if (can_increment_exclusive_lock())
        {
            increment_exclusive_lock();
            return true;
        }
        if (can_start_exclusive_lock())
        {
            start_exclusive_lock();
            return true;
        }
        return false;
    }

    void SharedRecursiveMutex::Unlock()
    {
        {
            std::unique_lock sync_lock(m_mtx);
            decrement_exclusive_lock();
        }
        m_cond_var.notify_all();
    }

    void SharedRecursiveMutex::LockShared()
    {
        std::unique_lock sync_lock(m_mtx);
        m_cond_var.wait(sync_lock, [this] { return can_lock_shared(); });
        increment_shared_lock();
    }

    bool SharedRecursiveMutex::TryLockShared()
    {
        std::unique_lock sync_lock(m_mtx);
        if (can_lock_shared())
        {
            increment_shared_lock();
            return true;
        }
        return false;
    }

    void SharedRecursiveMutex::UnlockShared()
    {
        {
            std::unique_lock sync_lock(m_mtx);
            decrement_shared_lock();
        }
        m_cond_var.notify_all();
    }

}
