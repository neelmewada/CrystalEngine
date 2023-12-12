#pragma once

namespace CE::RPI
{
	namespace Limits
	{
		constexpr u32 ModelLodCountMax = 8;

		namespace Pipeline
		{
			constexpr u32 MaxColorAttachmentCount = 8;
			constexpr u32 DrawListTagCount = 64;
		}
	}
} // namespace CE::RPI
