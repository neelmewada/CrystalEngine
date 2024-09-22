#pragma once

#if PLATFORM_MAC

namespace CE
{
    class CORE_API MacMemory
    {
    public:

        MacMemory() = delete;

        static void* AlignedAlloc(SIZE_T size, SIZE_T alignment);

        static void AlignedFree(void* block);
        
        static void* AlignedRealloc(void* block, SIZE_T size, SIZE_T alignment);
        
        static SIZE_T GetAlignedBlockSize(void* block, SIZE_T alignment, SIZE_T offset = 0);
    };

    typedef MacMemory PlatformMemory;
}
#endif
