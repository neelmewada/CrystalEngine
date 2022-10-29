
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

    void WindowsMemory::AlignedFree(void* block)
    {
#if CE_DEBUG
        _aligned_free_dbg(block);
#else
        _aligned_free(block);
#endif
    }

}
