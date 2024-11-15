
#include "CoreMinimal.h"

namespace CE
{

    u32 LinuxTLS::AllocTLS()
    {
        pthread_key_t slotKey = 0;
        if (pthread_key_create(&slotKey, NULL) != 0)
        {
            slotKey = 0xFFFFFFFF;
        }
        return (u32)slotKey;
    }

    void LinuxTLS::FreeTLS(u32 slotIndex)
    {
        pthread_key_delete((pthread_key_t)slotIndex);
    }

    void* LinuxTLS::GetTLSValue(u32 slotIndex)
    {
        return pthread_getspecific((pthread_key_t)slotIndex);
    }

    void LinuxTLS::SetTLSValue(u32 slotIndex, void* value)
    {
        pthread_setspecific((pthread_key_t)slotIndex, value);
    }
    
}
