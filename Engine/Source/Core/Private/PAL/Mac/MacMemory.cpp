
#include "CoreTypes.h"
#include "PAL/Mac/MacMemory.h"

#include <stdlib.h>

namespace CE
{
    
    void* MacMemory::AlignedAlloc(SIZE_T size, SIZE_T alignment)
    {
        return aligned_alloc(alignment, size);
    }

    void MacMemory::AlignedFree(void* block)
    {
        free(block);
    }

    void* MacMemory::AlignedRealloc(void* block, SIZE_T size, SIZE_T alignment)
    {
        void* newBlock = aligned_alloc(alignment, size);
        memcpy(newBlock, block, size);
        AlignedFree(block);
        return newBlock;
    }

    SIZE_T MacMemory::GetAlignedBlockSize(void* block, SIZE_T alignment, SIZE_T offset)
    {
        return 0;
    }
    
}

