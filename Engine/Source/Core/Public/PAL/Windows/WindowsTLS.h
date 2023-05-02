#pragma once

namespace CE
{

    class CORE_API WindowsTLS
    {
    public:

        static u32 AllocTLS();

        static void FreeTLS(u32 slotIndex);

        static void* GetTLSValue(u32 slotIndex);

        static void SetTLSValue(u32 slotIndex, void* value);
        
    };

    typedef WindowsTLS PlatformTLS;
    
} // namespace CE
