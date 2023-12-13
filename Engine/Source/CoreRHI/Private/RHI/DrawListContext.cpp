#include "CoreRHI.h"

namespace CE::RHI
{

	DrawList& DrawListContext::GetDrawListForTag(u8 tag)
	{
		static DrawList empty{};
		if (tag >= drawListsByTag.GetSize())
			return empty;

		return drawListsByTag[tag];
	}

} // namespace CE::RHI
