#pragma once

#include "Stream.h"

namespace CE
{

    class CORE_API MemoryStream : public Stream
    {
    public:
        MemoryStream(u32 sizeToAllocate);
        MemoryStream(void* data, u32 length, Permissions permissions = Permissions::ReadOnly);

        virtual ~MemoryStream();

        void Serialize(void* value, u64 length) override;
        void Serialize(u8& byte) override;
        void Serialize(String& string) override;
        
        bool IsOpen() override;
        void Close() override;
        bool CanRead() override;
        bool CanWrite() override;
        u64 GetLength() override;
        u64 GetMaximumSize() override;
        bool HasHardSizeLimit() override;

        void* GetRawDataPtr() const override
        {
            return data;
        }

        u32 GetCurrentPosition() const
        {
            return offset;
        }

        void Seek(u32 seekPos, SeekMode seekMode = SeekMode::Begin)
        {
            switch (seekMode)
            {
            case SeekMode::Begin: offset = seekPos; break;
            case SeekMode::Current: offset += seekPos; break;
            case SeekMode::End: offset = GetMaximumSize() - 1 - seekPos; break;
            }
        }

    private:
        u8* data = nullptr;
        u32 bufferSize = 0;
        u32 dataSize = 0;
        
        u32 offset = 0;

        Permissions permissions = Permissions::ReadOnly;
        bool isAllocated = false;
        
    };

    
} // namespace CE
