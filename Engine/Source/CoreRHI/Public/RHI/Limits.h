#pragma once

namespace CE::RHI
{
	namespace Limits
	{
		constexpr u32 ModelLodCountMax = 8;

		namespace Pipeline
		{
			//! @brief Max number of vertex attributes
			constexpr u32 MaxVertexAttribCount = 16;

			//! @brief Max number of vertex buffer input slots.
			constexpr u32 MaxVertexInputSlotCount = 16;

			//! @brief Max number of back buffers (images) that can be used in swap chain.
			constexpr u32 MaxSwapChainImageCount = 2;

			/// @brief Max number of simultaneous frames that can be rendered simultaneously (triple buffering, etc).
			constexpr u32 MaxFramesInFlight = MaxSwapChainImageCount - 1;

			//! @brief Max number of color attachment counts.
			constexpr u32 MaxColorAttachmentCount = 8;

			//! @brief Max number of output attachments. Resolve + Color + DepthStencil
			constexpr u32 MaxRenderAttachmentCount = 2 * MaxColorAttachmentCount + 1;

			//! @brief Max number of subpasses a render pass can have
			constexpr u32 MaxSubPassCount = 12;

			/// @brief Max number of DrawList Tags
			constexpr u32 DrawListTagCount = 64;

			/// @brief Max number of shader resource groups that can be bound
			constexpr u32 MaxShaderResourceGroupCount = 8;

			/// @brief Max byte size of root constants
			constexpr u8 MaxRootConstantSize = 64;
		}
	}
} // namespace CE::RPI
