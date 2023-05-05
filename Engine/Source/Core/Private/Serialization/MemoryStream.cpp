
#include "CoreMinimal.h"

namespace CE
{

    MemoryStream::MemoryStream(u32 sizeToAllocate)
    {
        ASSERT(sizeToAllocate < 2_GB, "Memory Streams only support size up to 2 GB.");
        if (sizeToAllocate == 0)
            return;

        dataSize = 0;
        bufferSize = sizeToAllocate;
        data = new u8[bufferSize];
        data[0] = 0;
        
        isAllocated = true;
        permissions = Permissions::ReadWrite;
        offset = 0;
        isReading = false; // set to writing mode
    }

    MemoryStream::MemoryStream(void* data, u32 length, Permissions permissions)
    {
        ASSERT(length < 2_GB, "Memory Streams only support size up to 2 GB.");

        dataSize = length;
        bufferSize = length;
        this->data = (u8*)data;

        isAllocated = false;
        this->permissions = permissions;
        offset = 0;
        isReading = true; // set to reading mode
    }

    MemoryStream::~MemoryStream()
    {
        if (isAllocated)
            delete data;
        data = nullptr;
        dataSize = bufferSize = 0;
        offset = 0;
    }

    void MemoryStream::Serialize(void* value, u64 length)
    {
        if (offset >= bufferSize)
            return;
        
        if (IsReading() && CanRead()) // Reading
        {
            if (offset + length > bufferSize)
                length = bufferSize - offset;
            memcpy(value, data + offset, length);
            offset += length;
        }
        else if (CanWrite()) // Writing
        {
            if (offset + length > bufferSize)
                length = bufferSize - offset;
            memcpy(data + offset, value, length);
            offset += length;
            dataSize = Math::Max(dataSize, offset);
        }
    }

    void MemoryStream::Serialize(u8& byte)
    {
        if (offset >= bufferSize)
            return;

        if (IsBinarySerialization()) // Binary
        {
            Serialize(&byte, 1);
        }
        else // ASCII
        {
            if (offset >= bufferSize)
                return;
            
            if (IsReading()) // Reading
            {
                memcpy(&byte, data + offset, 1);
                offset += 1;
            }
            else // Writing
            {
                memcpy(data + offset, &byte, 1);
                offset += 1;
                dataSize = Math::Max(dataSize, offset);
            }
        }
    }

    void MemoryStream::Serialize(String& string)
    {
        if (offset >= bufferSize)
            return;
        
        if (IsReading() && CanRead()) // Reading
        {
            if (IsBinarySerialization()) // Binary
            {
                u32 startPos = offset;
                u32 endPos;
                for (endPos = startPos; endPos < bufferSize; endPos++)
                {
                    if (data[endPos] == 0)
                        break;
                }
                int length = endPos - startPos + 1;
                if (startPos < endPos)
                {
                    string.Reserve(length);
                    memcpy(string.GetCString(), data + offset, length);
                    string[length] = 0;
                    offset = endPos + 1;
                }
            }
            else // ASCII
            {
                u32 startPos = offset;
                u32 endPos;
                u32 finalOffset = offset;
                for (endPos = startPos; endPos < bufferSize; endPos++)
                {
                    if (endPos < bufferSize - 1 && data[endPos] == '\r' && data[endPos + 1] == '\n')
                    {
                        finalOffset = endPos + 2;
                        break;
                    }
                    if (data[endPos] == '\n')
                    {
                        finalOffset = endPos + 1;
                        break;
                    }
                }
                int length = endPos - startPos + 1;
                if (startPos < endPos)
                {
                    string.Reserve(length);
                    memcpy(string.GetCString(), data + offset, length);
                    string[length] = 0;
                    offset = finalOffset;
                }
            }
        }
        else if (CanWrite() && string.GetLength() > 0) // Writing
        {
            int length = string.GetLength();
            if (IsBinarySerialization()) // Binary
            {
                if (offset + length + 1 > bufferSize)
                    length = bufferSize - offset - 1;
                memcpy(data + offset, string.GetCString(), length + 1); // +1 for null terminator
                *(data + offset + length) = '\0'; // Force a null terminator at the end
                offset += length + 1;
                dataSize = Math::Max(dataSize, offset);
            }
            else // ASCII
            {
                if (offset + length > bufferSize)
                    length = bufferSize - offset;
                memcpy(data + offset, string.GetCString(), length);
                offset += length;
                dataSize = Math::Max(dataSize, offset);
            }
        }
    }

    bool MemoryStream::IsOpen()
    {
        return data != nullptr;
    }

    void MemoryStream::Close()
    {
        if (isAllocated)
            delete data;
        data = nullptr;
        dataSize = bufferSize = 0;
        offset = 0;
    }

    bool MemoryStream::CanRead()
    {
        return permissions == Permissions::ReadOnly || permissions == Permissions::ReadWrite;
    }

    bool MemoryStream::CanWrite()
    {
        return permissions == Permissions::WriteOnly || permissions == Permissions::ReadWrite;
    }

    u64 MemoryStream::GetLength()
    {
        return dataSize;
    }

    u64 MemoryStream::GetMaximumSize()
    {
        return bufferSize;
    }

    bool MemoryStream::HasHardSizeLimit()
    {
        return true;
    }
    
} // namespace CE
