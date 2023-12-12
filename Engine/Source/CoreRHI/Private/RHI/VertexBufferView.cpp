#include "CoreRHI.h"

namespace CE::RHI
{

    VertexBufferView::VertexBufferView(RHI::Buffer* buffer, u32 byteOffset, u32 byteCount, u32 vertexStride)
		: buffer(buffer)
		, byteOffset(byteOffset)
		, byteCount(byteCount)
		, vertexStride(vertexStride)
    {

    }

} // namespace CE::RHI
