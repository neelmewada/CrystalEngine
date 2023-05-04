
#include "CoreMinimal.h"

namespace CE
{
    Stream& Stream::operator<<(String& string)
    {
        if (IsLoading()) // Reading
        {
            u32 length = 0;
            *this << length;
            if (length > 0)
            {
                string = String(length);
                Serialize(const_cast<char*>(string.GetCString()), length);
                string.StringLength = length;
                string.Buffer[length] = 0; // Add null terminator
            }
            else
            {
                string = "";
            }
        }
        else // Writing
        {
            u32 length = string.GetLength();
            if (length > 0)
            {
                *this << length;
                Serialize(const_cast<char*>(string.GetCString()), length);
            }
        }
        return *this;
    }

    Stream& Stream::operator<<(char& ch)
    {
        Serialize(&ch, 1);
        return *this;
    }

    Stream& Stream::operator<<(UUID& uuid)
    {
        if (IsLoading()) // Reading
        {
            u64 value = 0;
            *this << value;
            uuid = value;
        }
        else // Writing
        {
            u64 value = uuid;
            *this << value;
        }
        return *this;
    }

    Stream& Stream::operator<<(Name& name)
    {
        if (IsLoading()) // Reading
        {
            String stringValue = "";
            *this << stringValue;
            name = stringValue;
        }
        else // Writing
        {
            String stringValue = name.GetString();
            *this << stringValue;
        }
        return *this;
    }

    Stream& Stream::SerializeInSwappedByteOrder(void* value, u32 length)
    {
        if (IsLoading()) // Reading
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

