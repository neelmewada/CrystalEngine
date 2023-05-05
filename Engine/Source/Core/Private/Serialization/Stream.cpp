
#include "CoreMinimal.h"

namespace CE
{
    Stream& Stream::operator<<(u16& value)
    {
        if (IsBinarySerialization()) // Binary
        {
            Serialize(&value, sizeof(u16));
        }
        else // ASCII
        {
            if (IsReading()) // Reading
            {
                
            }
            else // Writing
            {
                auto str = String::Format("{}", value);
                Serialize(str);
            }
        }
        return *this;
    }

    Stream& Stream::SerializeInSwappedByteOrder(void* value, u32 length)
    {
        if (IsReading()) // Reading
        {
            Serialize(value, length);
            SwapBytes(value, length);
        }
        else // Writing
        {
            SwapBytes(value, length);
            Serialize(value, length);
            SwapBytes(value, length);
        }
        return *this;
    }

    void Stream::SwapBytes(void* value, u32 length)
    {
        u8* ptr = (u8*)value;
        s32 top = length - 1;
        s32 bottom = 0;
        while (bottom < top)
        {
            Swap(ptr[bottom++], ptr[top--]);
        }
    }
} // namespace CE

