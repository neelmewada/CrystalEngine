
#include "CoreMinimal.h"

namespace CE
{

    MemoryStream::MemoryStream(u32 sizeToAllocate)
    {
        ASSERT(sizeToAllocate < 2_GB, "Memory Streams only support size up to 2 GB.");
        if (sizeToAllocate == 0)
            return;
        
        bufferSize = sizeToAllocate;
        data = new u8[bufferSize];
        data[0] = 0;
        
        isAllocated = true;
        permissions = Permissions::ReadWrite;
        offset = 0;
        isLoading = false; // set to writing mode
    }

    MemoryStream::MemoryStream(void* data, u32 length, Permissions permissions)
    {
        ASSERT(length < 2_GB, "Memory Streams only support size up to 2 GB.");

        bufferSize = length;
        this->data = (u8*)data;

        isAllocated = false;
        this->permissions = permissions;
        offset = 0;
        isLoading = true; // set to reading mode
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
        
        if (IsLoading() && CanRead()) // Reading
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
