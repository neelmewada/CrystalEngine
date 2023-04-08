#pragma once

#include <thread>

namespace CE
{

    using ThreadId = std::thread::id;

    class CORE_API Thread
    {
    public:

        inline static ThreadId GetCurrentThreadId()
        {
            return std::this_thread::get_id();
        }
        
    private:
    };
    
} // namespace CE
