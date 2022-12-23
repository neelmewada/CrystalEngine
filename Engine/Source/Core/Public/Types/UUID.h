#pragma once

#include "Types/CoreTypeDefs.h"

#include "Algorithm/Hash.h"
#include "Containers/String.h"
#include "Containers/StringView.h"

namespace CE
{

    class CORE_API UUID
    {
    public:
        UUID();
        
        UUID(u64 value);
        
        UUID(const UUID&) = default;
        
        CE_INLINE operator u64() const
        {
            return uuid;
        }
        
    private:
        u64 uuid;
    };
    
} // namespace CE
