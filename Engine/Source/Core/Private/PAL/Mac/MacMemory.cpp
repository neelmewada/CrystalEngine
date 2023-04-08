
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
    
}

