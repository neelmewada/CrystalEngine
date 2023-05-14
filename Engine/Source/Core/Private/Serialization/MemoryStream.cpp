
#include "CoreMinimal.h"

namespace CE
{

    /* ***************************************************************************************************************
     *  Memory Ascii Stream
     */

    MemoryStream::MemoryStream(u32 sizeToAllocate, b8 autoResize)
    {
        ASSERT(sizeToAllocate < 2_GB, "Memory Streams only support size up to 2 GB.");
        
        if (sizeToAllocate == 0)
            return;

        dataSize = 0;
        bufferSize = sizeToAllocate;
        data = new u8[bufferSize];
        data[0] = 0;
        
        isAllocated = true;
        this->autoResize = autoResize;
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
        autoResize = false;
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
            delete[] data;
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

    bool MemoryStream::IsOutOfBounds()
    {
        return Stream::IsOutOfBounds() || offset >= dataSize;
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
        if (autoResize && CanResize() && offset + length >= bufferSize)
        {
            Resize(offset + length + autoResizeIncrement);
        }
        memcpy(data + offset, inData, length);
        offset += length;
        dataSize = Math::Max(dataSize, offset);
    }

    s64 MemoryStream::Read(void* outData, u64 length)
    {
        memcpy(outData, data + offset, length);
        offset += length;
        return length;
    }

    bool MemoryStream::CanResize() const
    {
        return isAllocated;
    }

    bool MemoryStream::Resize(u32 newSize)
    {
        if (!CanResize())
            return false;

        // Allocate new buffer
        u8* newBuffer = new u8[newSize];

        // Copy old data & free old buffer
        memcpy(newBuffer, data, bufferSize);
        delete[] data;

        // Update variables
        data = newBuffer;
        bufferSize = newSize;
        dataSize = Math::Min(dataSize, bufferSize);
        return true;
    }
    
} // namespace CE
