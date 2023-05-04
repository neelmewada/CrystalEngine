
#include "CoreMinimal.h"

namespace CE
{

    MemoryStream::MemoryStream(u32 sizeToAllocate)
    {
        ASSERT(sizeToAllocate < 2_GB, "Memory Streams only support streams of size up to 2 GB.");
        if (sizeToAllocate == 0)
            return;
        
        bufferSize = sizeToAllocate;
        data = new u8[bufferSize];
        data[0] = 0;
        
        isAllocated = true;
        permissions = StreamMode::ReadWrite;
        offset = 0;
        isLoading = false; // set to writing mode
    }

    MemoryStream::~MemoryStream()
    {
        if (MemoryStream::IsOpen())
            MemoryStream::Close();
    }

    void MemoryStream::Serialize(void* value, u64 length)
    {
        if (offset >= bufferSize)
            return;
        
        if (IsLoading()) // Reading
        {
            if (offset + length > bufferSize)
                length = bufferSize - offset;
            memcpy(value, data + offset, length);
            offset += length;
        }
        else // Writing
        {
            if (offset + length > bufferSize)
                length = bufferSize - offset;
            memcpy(data + offset, value, length);
            offset += length;
            dataSize = offset;
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
        return Stream::CanRead();
    }

    bool MemoryStream::CanWrite()
    {
        return Stream::CanWrite();
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
