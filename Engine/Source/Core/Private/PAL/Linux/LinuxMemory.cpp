
#include "CoreTypes.h"
#include "PAL/Linux/LinuxMemory.h"

#include <stdlib.h>

namespace CE
{
    
    void* LinuxMemory::AlignedAlloc(SIZE_T size, SIZE_T alignment)
    {
        return aligned_alloc(alignment, size);
    }

    void LinuxMemory::AlignedFree(void* block)
    {
        free(block);
    }

    void* LinuxMemory::AlignedRealloc(void* block, SIZE_T size, SIZE_T alignment)
    {
        void* newBlock = aligned_alloc(alignment, size);
        memcpy(newBlock, block, size);
        AlignedFree(block);
        return newBlock;
    }

    SIZE_T LinuxMemory::GetAlignedBlockSize(void* block, SIZE_T alignment, SIZE_T offset)
    {
        return 0;
    }
    
}

