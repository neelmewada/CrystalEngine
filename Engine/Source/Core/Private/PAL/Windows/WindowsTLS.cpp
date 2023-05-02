
#include <ObjectArray.h>

#include "CoreMinimal.h"

namespace CE
{

    u32 WindowsTLS::AllocTLS()
    {
        return TlsAlloc();
    }

    void WindowsTLS::FreeTLS(u32 slotIndex)
    {
        TlsFree(slotIndex);
    }

    void* WindowsTLS::GetTLSValue(u32 slotIndex)
    {
        return TlsGetValue(slotIndex);
    }

    void WindowsTLS::SetTLSValue(u32 slotIndex, void* value)
    {
        TlsSetValue(slotIndex, value);
    }
    
}
