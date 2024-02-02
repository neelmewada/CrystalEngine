#pragma once

namespace CE::RHI
{
	/*
	 * Copyright (c) Contributors to the Open 3D Engine Project.
	 * Used under MIT license. https://github.com/o3de/o3de/blob/development/LICENSE_MIT.TXT
	 */

	/// @brief DrawPacket is a collection of DrawItems that represent a single mesh. DrawPacket is allocated as a single contiguous memory and all the pointers
	/// point inside the same contiguous memory. The actual memory allocated for a DrawPacket stretches beyond sizeof(DrawPacket).
	/// 
	/// Each DrawItem represent a single pass on the mesh and is associated with a DrawListTag which denotes the DrawList the DrawItem is associated to.
	/// DrawListTag can be used to determine the pass for ex: Shadow Pass, Forward+ Pass, Transparent pass, etc.
	class CORERHI_API DrawPacket final : public IntrusiveBase
	{
		CE_NO_COPY(DrawPacket)
	public:

		DrawPacket() = default;

		void operator delete(void* p, size_t size);

		DrawItemProperties GetDrawItemProperties(SIZE_T index);

		inline SIZE_T GetDrawItemCount() { return drawItemCount; }

		inline DrawListTag GetDrawListTag(int index) const { return drawListTags[index]; }
        
        DrawListMask drawListMask{};

		u8 drawItemCount = 0;
		u8 vertexBufferViewCount = 0;
		u8 rootConstantSize = 0;
		u8 shaderResourceGroupCount = 0;
		u8 uniqueShaderResourceGroupCount = 0;
		u8 scissorCount = 0;
		u8 viewportCount = 0;

		IAllocator* allocator = nullptr;

		/// @brief List of draw items. Count = drawItemCount.
		DrawItem* drawItems = nullptr;

		/// @brief List of draw list tags associated with the draw item index.
		const DrawListTag* drawListTags = nullptr;

		/// @brief List of draw filter masks associated with the draw item index.
		const DrawFilterMask* drawFilterMasks = nullptr;

		const u8* rootConstants = nullptr;

		ShaderResourceGroup** shaderResourceGroups = nullptr;

		ShaderResourceGroup** uniqueShaderResourceGroups = nullptr;

		/// @brief Vertex buffer views across all draw items
		const VertexBufferView* vertexBufferViews = nullptr;

		/// @brief Scissor states for all draw items.
		const ScissorState* scissors = nullptr;

		/// @brief Viewport states for all draw items.
		const ViewportState* viewports = nullptr;

	private:

		friend class DrawPacketBuilder;
	};
    
} // namespace CE::RPI
