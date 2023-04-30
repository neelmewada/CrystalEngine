#pragma once

#include <thread>

namespace CE
{

    using ThreadId = std::thread::id;

    class CORE_API Thread
    {
    public:
        Thread() : thread({})
        {
            threadId = thread.get_id();
        }

        Thread(auto func) : thread(func)
        {
            threadId = thread.get_id();
        }

        inline static ThreadId GetCurrentThreadId()
        {
            return std::this_thread::get_id();
        }

        inline static void SleepFor(SIZE_T milliSeconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliSeconds));
        }

        ThreadId GetId() const
        {
            return threadId;
        }

        bool IsJoinable() const
        {
            return thread.joinable();
        }

        void Join()
        {
            thread.join();
        }

        void Detach()
        {
            thread.detach();
        }
        
    private:
        ThreadId threadId{};
        std::thread thread{};
    };
    
} // namespace CE
