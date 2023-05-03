
#include "CoreMinimal.h"

namespace CE
{
    Stream& Stream::operator<<(String& name)
    {
        u32 length = name.GetLength();
        if (length > 0)
        {
            *this << length;
            Serialize(name.GetCString(), name.GetLength());
        }
        return *this;
    }

    Stream& Stream::operator<<(char& ch)
    {
        Serialize(ch);
        return *this;
    }
    
} // namespace CE

