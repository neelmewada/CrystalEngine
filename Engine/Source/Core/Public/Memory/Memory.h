#pragma once

#include "Types/CoreTypeDefs.h"
#include "Misc/CoreDefines.h"

#include "PAL/Common/PlatformMemory.h"

#include <memory>

namespace CE
{
    class CORE_API Memory
    {
    public:
        CE_STATIC_CLASS(Memory);
        
        static inline void* SystemMalloc(SIZE_T size)
        {
            return ::malloc(size);
        }

        static inline void SystemFree(void* pointer)
        {
            ::free(pointer);
        }

        static inline void* Malloc(SIZE_T size)
        {
            void* block = SystemMalloc(size);
            Zero(block, size);
            return block;
        }

        static inline void Free(void* pointer)
        {
            SystemFree(pointer);
        }

        static inline void Zero(void* block, SIZE_T size)
        {
            memset(block, 0, size);
        }

        /* Allocates memory with the Size aligned to specified Alignment. IMPORTANT: Alignment should always be a power of 2 */
        static inline void* Malloc(SIZE_T size, SIZE_T alignment)
        {
            auto AlignedSize = GetAlignedSize(size, alignment);
            return Malloc(AlignedSize);
        }

        /* Allocates memory with base address & size aligned to specified Alignment. IMPORTANT: Alignment should always be a power of 2 */
        static inline void* AlignedAlloc(SIZE_T size, SIZE_T alignment)
        {
            return PlatformMemory::AlignedAlloc(size, alignment);
        }

        static inline void AlignedFree(void* block)
        {
            PlatformMemory::AlignedFree(block);
        }

        /* Returns a Size that is Aligned to the given Alignment. IMPORTANT: Alignment should always be a power of 2 */
        static inline SIZE_T GetAlignedSize(SIZE_T baseSize, SIZE_T alignment)
        {
            return (baseSize + alignment - 1) & ~(alignment - 1);
        }
    };
}
