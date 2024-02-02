#include "CoreRHI.h"

namespace CE::RHI
{

    VertexBufferView::VertexBufferView(RHI::Buffer* buffer, u64 byteOffset, u64 byteCount, u32 vertexStride)
		: buffer(buffer)
		, byteOffset(byteOffset)
		, byteCount(byteCount)
		, vertexStride(vertexStride)
    {

    }

} // namespace CE::RHI
