#include "CoreRHI.h"

namespace CE::RHI
{
    BufferView::BufferView(RHI::Buffer* buffer, u64 byteOffset, u64 byteCount)
        : buffer(buffer)
        , byteOffset(byteOffset)
        , byteCount(byteCount)
    {
        if (buffer)
        {
            if (this->byteCount == 0)
            {
                this->byteCount = buffer->GetBufferSize();
            }
        }
    }
} // namespace CE::RHI
