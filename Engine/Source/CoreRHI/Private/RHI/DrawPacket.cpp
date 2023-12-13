#include "CoreRHI.h"

namespace CE::RHI
{
	void DrawPacket::operator delete(void* p, size_t size)
	{
		reinterpret_cast<const DrawPacket*>(p)->allocator->AlignedFree(p);
	}

	DrawItemProperties DrawPacket::GetDrawItemProperties(SIZE_T index)
	{
		CE_ASSERT(index < GetDrawItemCount(), "GetDrawItemProperties(): index {} out of bounds {}", index, GetDrawItemCount());

		return DrawItemProperties(&drawItems[index], drawFilterMasks[index]);
	}

} // namespace CE::RHI
