#pragma once

#include "Mutex.h"
#include "Thread.h"
#include "Delegates/Delegate.h"

namespace CE
{

    class CORE_API TlsAutoCleanup
    {
    public:
        virtual ~TlsAutoCleanup()
        {}

        void RegisterForAutoCleanup();

    protected:
        u32 slotIndex = 0;
    };

    class CORE_API TlsInitializer
    {
    public:

        static TlsAutoCleanup* Get(std::function<TlsAutoCleanup*()> createInstance, u32& tlsSlot);
        
    };
    
    template<class T>
    class ThreadSingleton : public TlsAutoCleanup
    {
    protected:
        ThreadSingleton() : threadId(Thread::GetCurrentThreadId())
        {}

    public:

        ThreadSingleton(const ThreadSingleton& copy) = delete;
        ThreadSingleton operator=(const ThreadSingleton& copy) = delete;

        FORCE_INLINE static T& Get()
        {
            return *(T*)TlsInitializer::Get([]{ return (TlsAutoCleanup*)new T(); }, tlsSlotIndex);
        }

        ThreadId GetThreadId() const
        {
            return threadId;
        }
    

    private:

        u32 slotIndex;
        ThreadId threadId;

        static u32 tlsSlotIndex;
        static Mutex mutex;
    };

    template<typename T>
    u32 ThreadSingleton<T>::tlsSlotIndex = 0xFFFFFFFF;

    template<typename T>
    Mutex ThreadSingleton<T>::mutex{};

} // namespace CE
