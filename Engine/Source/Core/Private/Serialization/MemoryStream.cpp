
#include "CoreMinimal.h"

namespace CE
{

    /* ***************************************************************************************************************
     *  Memory Ascii Stream
     */

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
    }

    MemoryStream::~MemoryStream()
    {
        MemoryStream::Close();
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

    u64 MemoryStream::GetCapacity()
    {
        return bufferSize;
    }

    bool MemoryStream::HasHardSizeLimit()
    {
        return true;
    }

    void MemoryStream::SetOutOfBounds()
    {
        offset = bufferSize;
    }

    void MemoryStream::Seek(s64 seekPos, SeekMode seekMode)
    {
        switch (seekMode)
        {
        case SeekMode::Begin: offset = (s32)seekPos; break;
        case SeekMode::Current: offset += (s32)seekPos; break;
        case SeekMode::End: offset = GetCapacity() - 1 + (s32)seekPos; break;
        }
    }

    void MemoryStream::Write(const void* inData, u64 length)
    {
        memcpy(data + offset, inData, length);
        offset += length;
    }

    void MemoryStream::Read(void* outData, u64 length)
    {
        memcpy(outData, data + offset, length);
        offset += length;
        dataSize = Math::Max(dataSize, offset);
    }

} // namespace CE
