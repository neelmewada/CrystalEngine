#pragma once

#if PLATFORM_LINUX

namespace CE
{
    class CORE_API LinuxMemory
    {
    public:

        LinuxMemory() = delete;

        static void* AlignedAlloc(SIZE_T size, SIZE_T alignment);

        static void AlignedFree(void* block);
        
        static void* AlignedRealloc(void* block, SIZE_T size, SIZE_T alignment);
        
        static SIZE_T GetAlignedBlockSize(void* block, SIZE_T alignment, SIZE_T offset = 0);
    };

    typedef LinuxMemory PlatformMemory;
}
#endif
