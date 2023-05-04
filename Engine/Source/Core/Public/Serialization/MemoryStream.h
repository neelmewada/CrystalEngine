#pragma once

#include "Stream.h"

namespace CE
{

    class CORE_API MemoryStream : public Stream
    {
    public:
        MemoryStream(u32 sizeToAllocate);

        virtual ~MemoryStream();

        void Serialize(void* value, u64 length) override;
        
        bool IsOpen() override;
        void Close() override;
        bool CanRead() override;
        bool CanWrite() override;
        u64 GetLength() override;
        u64 GetMaximumSize() override;
        bool HasHardSizeLimit() override;

        u32 GetCurrentPosition() const
        {
            return offset;
        }

        void Seek(u32 seekPos)
        {
            offset = seekPos;
        }

    private:
        u8* data = nullptr;
        u32 bufferSize = 0;
        u32 dataSize = 0;
        
        s32 offset = 0;

        StreamMode permissions = StreamMode::Default;
        bool isAllocated = false;
        
    };

    
} // namespace CE
