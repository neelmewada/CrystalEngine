#pragma once

#include <thread>

namespace CE
{

    using ThreadId = std::thread::id;

    class CORE_API Thread
    {
    public:
        Thread() : thread({})
        {}

        Thread(auto func) : thread(func)
        {}

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
            return thread.get_id();
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
        std::thread thread{};
    };
    
} // namespace CE
