
#include "CoreMinimal.h"

namespace CE
{

    /* ***************************************************************************************************************
     *  Memory Ascii Stream
     */

    MemoryAsciiStream::MemoryAsciiStream(u32 sizeToAllocate)
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
    }

    MemoryAsciiStream::MemoryAsciiStream(void* data, u32 length, Permissions permissions)
    {
        ASSERT(length < 2_GB, "Memory Streams only support size up to 2 GB.");

        dataSize = length;
        bufferSize = length;
        this->data = (u8*)data;

        isAllocated = false;
        this->permissions = permissions;
        offset = 0;
    }

    MemoryAsciiStream::~MemoryAsciiStream()
    {
        MemoryAsciiStream::Close();
    }
    
    bool MemoryAsciiStream::IsOpen()
    {
        return data != nullptr;
    }

    void MemoryAsciiStream::Close()
    {
        if (isAllocated)
            delete data;
        data = nullptr;
        dataSize = bufferSize = 0;
        offset = 0;
    }

    bool MemoryAsciiStream::CanRead()
    {
        return permissions == Permissions::ReadOnly || permissions == Permissions::ReadWrite;
    }

    bool MemoryAsciiStream::CanWrite()
    {
        return permissions == Permissions::WriteOnly || permissions == Permissions::ReadWrite;
    }

    u64 MemoryAsciiStream::GetLength()
    {
        return dataSize;
    }

    u64 MemoryAsciiStream::GetCapacity()
    {
        return bufferSize;
    }

    bool MemoryAsciiStream::HasHardSizeLimit()
    {
        return true;
    }

    void MemoryAsciiStream::Write(const void* inData, u64 length)
    {
        memcpy(data + offset, inData, length);
        offset += length;
    }

    void MemoryAsciiStream::Read(void* outData, u64 length)
    {
        memcpy(outData, data + offset, length);
        offset += length;
        dataSize = Math::Max(dataSize, offset);
    }

    /* ***************************************************************************************************************
     *  Memory Binary Stream
     */

    MemoryBinaryStream::MemoryBinaryStream(u32 sizeToAllocate)
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
    }

    MemoryBinaryStream::MemoryBinaryStream(void* data, u32 length, Permissions permissions)
    {
        ASSERT(length < 2_GB, "Memory Streams only support size up to 2 GB.");

        dataSize = length;
        bufferSize = length;
        this->data = (u8*)data;

        isAllocated = false;
        this->permissions = permissions;
        offset = 0;
    }

    MemoryBinaryStream::~MemoryBinaryStream()
    {
        MemoryBinaryStream::Close();
    }

    bool MemoryBinaryStream::IsOpen()
    {
        return data != nullptr;
    }

    void MemoryBinaryStream::Close()
    {
        if (isAllocated)
            delete data;
        data = nullptr;
        dataSize = bufferSize = 0;
        offset = 0;
    }

    bool MemoryBinaryStream::CanRead()
    {
        return permissions == Permissions::ReadOnly || permissions == Permissions::ReadWrite;
    }

    bool MemoryBinaryStream::CanWrite()
    {
        return permissions == Permissions::WriteOnly || permissions == Permissions::ReadWrite;
    }

    u64 MemoryBinaryStream::GetLength()
    {
        return dataSize;
    }

    u64 MemoryBinaryStream::GetCapacity()
    {
        return bufferSize;
    }

    bool MemoryBinaryStream::HasHardSizeLimit()
    {
        return true;
    }

    void MemoryBinaryStream::Write(const void* inData, u64 length)
    {
        memcpy(data + offset, inData, length);
        offset += length;
    }

    void MemoryBinaryStream::Read(void* outData, u64 length)
    {
        memcpy(outData, data + offset, length);
        offset += length;
        dataSize = Math::Max(dataSize, offset);
    }
    
} // namespace CE
