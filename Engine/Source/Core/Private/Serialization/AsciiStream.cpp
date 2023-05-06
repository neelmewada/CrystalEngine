
#include "CoreMinimal.h"

namespace CE
{

    Stream& AsciiStream::operator<<(const String& string)
    {
        Write(string.GetCString(), string.GetLength());
        return *this;
    }

    Stream& AsciiStream::operator>>(String& string)
    {
        auto startPos = GetCurrentPosition(); // cache starting pos
        u8 byte = 0;
        
        while (byte != '\n' && !IsOutOfBounds()) // Read until new line or EOF
        {
            if (byte == '\r')
            {
                if ((byte = Read()) != '\n')
                    continue;
                Seek(-1, SeekMode::Current);
                break;
            }

            byte = Read();
        }

        auto endPos = GetCurrentPosition() - 1; // endPos will always be at \n or \r

        if (IsOutOfBounds() && byte != '\n')
        {
            endPos = GetCurrentPosition(); // Include last character if it is NOT a new line and is EOF
        }

        Seek(startPos, SeekMode::Begin); // Go back to start pos

        if (startPos < endPos)
        {
            u32 length = endPos - startPos;
            string.Reserve(length);
            Read(string.GetCString(), length);
            string[length] = 0;
        }
        
        return *this;
    }
    
} // namespace CE

