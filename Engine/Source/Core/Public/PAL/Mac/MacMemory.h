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
    };

    typedef MacMemory PlatformMemory;
}
#endif
