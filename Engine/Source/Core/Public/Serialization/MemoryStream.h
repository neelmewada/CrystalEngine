#pragma once

#include "Stream.h"

namespace CE
{
    class CORE_API MemoryStream : public Stream
    {
    public:
        MemoryStream(u32 sizeToAllocate, b8 autoResize = true);
        MemoryStream(void* data, u32 length, Permissions permissions = Permissions::ReadOnly);

        virtual ~MemoryStream();

		MemoryStream(MemoryStream&& move) noexcept;
        
        bool IsOpen() override;
        void Close() override;
        bool CanRead() override;
        bool CanWrite() override;
        u64 GetLength() override;
        u64 GetCapacity() override;
        bool HasHardSizeLimit() override;

        void* GetRawDataPtr() const override
        {
            return data;
        }

        u64 GetCurrentPosition() override
        {
            return offset;
        }

        bool IsOutOfBounds() override;
        void SetOutOfBounds() override;

        void Seek(s64 seekPos, SeekMode seekMode = SeekMode::Begin) override;

        void Write(const void* inData, u64 length) override;
        s64 Read(void* outData, u64 length) override;

        void Write(u8 inByte) override
        {
            Write(&inByte, 1);
        }

        bool CanResize() const override;

        void SetAutoResizeIncrement(u32 increment) override;

        bool Resize(u32 newSize);

    private:
        u8* data = nullptr;
        u32 bufferSize = 0;
        u32 dataSize = 0;
        
        u32 offset = 0;
        b8 autoResize = false;
        u32 autoResizeIncrement = 128;

        Stream::Permissions permissions = Stream::Permissions::ReadOnly;
        bool isAllocated = false;
    };

    
} // namespace CE
