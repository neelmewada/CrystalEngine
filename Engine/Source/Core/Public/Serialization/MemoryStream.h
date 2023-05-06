#pragma once

#include "Stream.h"

namespace CE
{
    namespace Internal
    {
        class CORE_API MemoryStreamBase
        {
        protected:
            
            u8* data = nullptr;
            u32 bufferSize = 0;
            u32 dataSize = 0;
        
            u32 offset = 0;

            Stream::Permissions permissions = Stream::Permissions::ReadOnly;
            bool isAllocated = false;
        };
    }

    class CORE_API MemoryAsciiStream : public AsciiStream, public Internal::MemoryStreamBase
    {
    public:
        MemoryAsciiStream(u32 sizeToAllocate);
        MemoryAsciiStream(void* data, u32 length, Permissions permissions = Permissions::ReadOnly);

        virtual ~MemoryAsciiStream();
        
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

        void Seek(s64 seekPos, SeekMode seekMode = SeekMode::Begin) override
        {
            switch (seekMode)
            {
            case SeekMode::Begin: offset = (s32)seekPos; break;
            case SeekMode::Current: offset += (s32)seekPos; break;
            case SeekMode::End: offset = GetCapacity() - 1 - (s32)seekPos; break;
            }
        }

        void Write(const void* inData, u64 length) override;
        void Read(void* outData, u64 length) override;

    private:
        
    };

    class CORE_API MemoryBinaryStream : public BinaryStream, public Internal::MemoryStreamBase
    {
    public:
        MemoryBinaryStream(u32 sizeToAllocate);
        MemoryBinaryStream(void* data, u32 length, Permissions permissions = Permissions::ReadOnly);

        virtual ~MemoryBinaryStream();
        
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

        void Seek(s64 seekPos, SeekMode seekMode = SeekMode::Begin) override
        {
            switch (seekMode)
            {
            case SeekMode::Begin: offset = (s32)seekPos; break;
            case SeekMode::Current: offset += (s32)seekPos; break;
            case SeekMode::End: offset = GetCapacity() - 1 - (s32)seekPos; break;
            }
        }

        void Write(const void* inData, u64 length) override;
        void Read(void* outData, u64 length) override;
    };

    // Always use Ascii as default stream
    using MemoryStream = MemoryAsciiStream;
    
} // namespace CE
