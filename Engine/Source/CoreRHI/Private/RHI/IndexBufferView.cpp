#include "CoreRHI.h"

namespace CE::RHI
{

	IndexBufferView::IndexBufferView(RHI::Buffer* buffer, u64 byteOffset, u64 byteCount, IndexFormat indexFormat)
		: buffer(buffer)
		, byteOffset(byteOffset)
		, byteCount(byteCount)
		, indexFormat(indexFormat)
	{

	}


} // namespace CE::RHI
