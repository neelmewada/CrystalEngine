#pragma once

#include <thread>

namespace CE
{

    typedef SIZE_T ThreadId;

    class CORE_API Thread
    {
    public:
        Thread() : thread({})
        {
            threadId = std::hash<std::thread::id>{}(thread.get_id());
        }

        Thread(auto func) : thread(func)
        {
            threadId = std::hash<std::thread::id>{}(thread.get_id());
        }

        inline static ThreadId GetCurrentThreadId()
        {
            return std::hash<std::thread::id>{}(std::this_thread::get_id());
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
