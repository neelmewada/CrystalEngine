#pragma once

#include "Types/CoreTypeDefs.h"

#include "Algorithm/Hash.h"
#include "Containers/String.h"
#include "Containers/StringView.h"

namespace CE
{
    CORE_API u64 GenerateRandomU64();

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
        
        CE_INLINE bool operator==(const UUID& other) const
        {
            return uuid == other.uuid;
        }
        
        CE_INLINE bool operator!=(const UUID& other) const
        {
            return uuid != other.uuid;
        }
        
        CE_INLINE bool operator==(const s64& other) const
        {
            return uuid == other;
        }
        
        CE_INLINE bool operator==(const u64& other) const
        {
            return uuid == other;
        }
        
        CE_INLINE bool operator!=(const s64& other) const
        {
            return uuid != other;
        }
        
        CE_INLINE bool operator!=(const u64& other) const
        {
            return uuid != other;
        }
        
        CE_INLINE bool operator==(const s32& other) const
        {
            return uuid == other;
        }
        
        CE_INLINE bool operator==(const u32& other) const
        {
            return uuid == other;
        }
        
        CE_INLINE bool operator!=(const s32& other) const
        {
            return uuid != other;
        }
        
        CE_INLINE bool operator!=(const u32& other) const
        {
            return uuid != other;
        }
        
    private:
        u64 uuid;
    };

    template<>
    inline SIZE_T GetHash<UUID>(const UUID& value)\
    {
        return (u64)value;
    }
    
} // namespace CE
