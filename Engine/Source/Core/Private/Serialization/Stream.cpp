
#include "CoreMinimal.h"

namespace CE
{
    Stream& Stream::operator<<(const String& string)
    {
        Write(string.GetCString(), string.GetLength());
        if (IsBinaryMode())
        {
            Write('\0'); // null terminator
        }
        return *this;
    }

    Stream& Stream::operator>>(String& string)
    {
        if (IsBinaryMode())
        {
            auto startPos = GetCurrentPosition();
        
            while (Read() != 0) {} // Read bytes until we encounter a null terminator

            auto endPos = GetCurrentPosition();
        
            if (startPos < endPos)
            {
                u32 count = endPos - startPos; // last will always be null terminator
                string.Reserve(count);
                Seek(startPos, SeekMode::Begin);
                Read(string.GetCString(), count);
            }
        }
        else if (IsAsciiMode())
        {
            auto startPos = GetCurrentPosition(); // cache starting pos
            u8 byte = 0;
            bool foundCarriageReturn = false;
        
            while (byte != '\n' && !IsOutOfBounds()) // Read until new line or EOF
            {
                if (byte == '\r')
                {
                    if ((byte = Read()) != '\n')
                        continue;
                    Seek(-1, SeekMode::Current);
                    foundCarriageReturn = true;
                    break;
                }

                byte = Read();
            }

            auto endPos = GetCurrentPosition() - 1; // endPos will always be at \n or \r
            bool isEOF = false;

            if (IsOutOfBounds() && byte != '\n')
            {
                isEOF = true;
                endPos = GetCurrentPosition(); // Include last character if it is NOT a new line and is EOF
            }

            Seek(startPos, SeekMode::Begin); // Go back to start pos

            if (startPos < endPos)
            {
                u32 length = endPos - startPos;
                string.Reserve(length);
                Read(string.GetCString(), length);
                string[length] = 0;

                if (foundCarriageReturn) // Add extra char for case: "/r/n"
                    endPos++;

                if (isEOF) // End Of File
                    SetOutOfBounds();
                else
                    Seek(endPos + 1, SeekMode::Begin); // Set offset to next line
            }
        }
        return *this;
    }

    Stream& Stream::operator<<(const u8& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(u8& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const u16& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(u16& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const u32& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(u32& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const u64& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(u64& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const s8& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(s8& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const s16& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(s16& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const s32& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(s32& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const s64& integer)
    {
        if (IsBinaryMode())
        {
            Write(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", integer);
        }
        return *this;
    }

    Stream& Stream::operator>>(s64& integer)
    {
        if (IsBinaryMode())
        {
            Read(&integer, sizeof(integer));
        }
        else if (IsAsciiMode())
        {
            
        }
        return *this;
    }

    Stream& Stream::operator<<(const f32& single)
    {
        if (IsBinaryMode())
        {
            Write(&single, sizeof(single));
        }
        else if (IsAsciiMode())
        {
            *this << String::Format("{}", single);
        }
        return *this;
    }

    Stream& Stream::operator>>(f32& single)
    {
        if (IsBinaryMode())
        {
            Read(&single, sizeof(single));
        }
        else if (IsAsciiMode())
        {
            
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

