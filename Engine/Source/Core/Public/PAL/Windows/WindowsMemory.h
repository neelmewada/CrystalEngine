#pragma once

#if PLATFORM_WINDOWS

#if DEBUG
#include <crtdbg.h>
#endif

namespace CE
{
    class CORE_API WindowsMemory
    {
    public:

        WindowsMemory() = delete;

        static inline void* AlignedAlloc(SIZE_T size, SIZE_T alignment)
        {
#if DEBUG
            return _aligned_malloc_dbg(size, alignment, __FILE__, __LINE__);
#else
            return _aligned_malloc(size, alignment);
#endif
        }

        static inline void AlignedFree(void* block)
        {
#if DEBUG
            _aligned_free_dbg(block);
#else
            _aligned_free(block);
#endif
        }
    };

    typedef WindowsMemory PlatformMemory;
}
#endif
