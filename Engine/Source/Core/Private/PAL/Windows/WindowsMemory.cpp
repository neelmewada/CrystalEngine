
#include "CoreTypes.h"
#include "PAL/Windows/WindowsMemory.h"

#include <Windows.h>

#if CE_DEBUG
#include <crtdbg.h>
#endif

namespace CE
{

    void* WindowsMemory::AlignedAlloc(SIZE_T size, SIZE_T alignment)
    {
#if CE_DEBUG
        return _aligned_malloc_dbg(size, alignment, __FILE__, __LINE__);
#else
        return _aligned_malloc(size, alignment);
#endif
    }

    void* WindowsMemory::AlignedRealloc(void* block, SIZE_T size, SIZE_T alignment)
    {
#if CE_DEBUG
        return _aligned_realloc_dbg(block, size, alignment, __FILE__, __LINE__);
#else
        return _aligned_realloc(block, size, alignment);
#endif
    }

    SIZE_T WindowsMemory::GetAlignedBlockSize(void* block, SIZE_T alignment, SIZE_T offset)
    {
#if CE_DEBUG
        return _aligned_msize_dbg(block, alignment, offset);
#else
        return _aligned_msize(block, alignment, offset);
#endif
    }

    void WindowsMemory::AlignedFree(void* block)
    {
#if CE_DEBUG
        _aligned_free_dbg(block);
#else
        _aligned_free(block);
#endif
    }

}
