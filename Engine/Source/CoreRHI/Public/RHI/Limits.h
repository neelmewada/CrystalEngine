#pragma once

namespace CE::RHI
{
	namespace Limits
	{
		constexpr u32 ModelLodCountMax = 8;

		namespace Pipeline
		{
			/// @brief Max number of vertex attributes
			constexpr u32 MaxVertexAttribCount = 8;

			/// @brief Max number of simultaneous frames rendered (triple buffering, etc)
			constexpr u32 MaxSimultaneousFramesInFlight = 4;

			constexpr u32 MaxColorAttachmentCount = 8;

			/// @brief Max number of DrawList Tags
			constexpr u32 DrawListTagCount = 64;
		}
	}
} // namespace CE::RPI
