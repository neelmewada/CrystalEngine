#pragma once

#if PLATFORM_WINDOWS

namespace CE
{
    class CORE_API WindowsMemory
    {
    public:

        WindowsMemory() = delete;

        static void* AlignedAlloc(SIZE_T size, SIZE_T alignment);

        static void* AlignedRealloc(void* block, SIZE_T size, SIZE_T alignment);

        static SIZE_T GetAlignedBlockSize(void* block, SIZE_T alignment, SIZE_T offset = 0);

        static void AlignedFree(void* block);
    };

    typedef WindowsMemory PlatformMemory;
}

#endif
