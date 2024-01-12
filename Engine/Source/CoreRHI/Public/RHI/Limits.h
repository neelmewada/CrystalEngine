#pragma once

namespace CE::RHI
{
	namespace Limits
	{
		constexpr u32 ModelLodCountMax = 8;

		namespace Pipeline
		{
			/// @brief Max number of vertex attributes
			constexpr u32 MaxVertexAttribCount = 16;

			/// @brief Max number of simultaneous frames that can be rendered (triple buffering, etc).
			/// For now we only support double buffering.
			constexpr u32 MaxFramesInFlight = 2;

			//! @brief Max number of attachments (color + input + depth stencil) that can be bound in a single pass
			constexpr u32 MaxBoundAttachmentCount = 8;

			//! @brief Max number of subpasses a render pass can have
			constexpr u32 MaxSubPassCount = 4;

			/// @brief Max number of DrawList Tags
			constexpr u32 DrawListTagCount = 64;

			/// @brief Max number of shader resource groups that can be bound
			constexpr u32 MaxShaderResourceGroupCount = 8;

			/// @brief Max byte size of root constants
			constexpr u8 MaxRootConstantSize = 64;
		}
	}
} // namespace CE::RPI
