#include "CoreMinimal.h"

namespace CE
{

    u32 Color::ToU32() const
    {
        return ((u32)(r * 255) << 24) | ((u32)(g * 255) << 16) | ((u32)(b * 255) << 8) | (u32)(a * 255);
    }
    
} // namespace CE
