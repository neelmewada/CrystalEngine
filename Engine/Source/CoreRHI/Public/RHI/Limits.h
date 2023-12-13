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

			/// @brief Max number of simultaneous frames that can be rendered (triple buffering, etc)
			constexpr u32 MaxSimultaneousFramesInFlight = 4;

			constexpr u32 MaxColorAttachmentCount = 8;

			/// @brief Max number of DrawList Tags
			constexpr u32 DrawListTagCount = 64;

			/// @brief Max number of shader resource groups that can be bound
			constexpr u32 MaxShaderResourceGroupCount = 8;
		}
	}
} // namespace CE::RPI
