#pragma once

namespace CE::RHI
{

	/// @brief DrawPacket is a collection of DrawItems that represent a single mesh. DrawPacket is allocated as a single contiguous memory and all the pointers
	/// point inside the same contiguous memory. The actual memory allocated for a DrawPacket stretches beyond sizeof(DrawPacket).
	/// 
	/// Each DrawItem represent a single pass on the mesh and is associated with a DrawListTag which denotes the DrawList the DrawItem is associated to.
	/// DrawListTag can be used to determine the pass for ex: Shadow Pass, Forward+ Pass, Transparent pass, etc.
	class CORERHI_API DrawPacket final : public IntrusiveBase
	{
		CE_NO_COPY(DrawPacket);
	public:

		DrawPacket() = default;

		void operator delete(void* p, size_t size);

		DrawItemProperties GetDrawItemProperties(SIZE_T index);

		inline SIZE_T GetDrawItemCount() { return drawItemCount; }

		IndexBufferView indexBufferView{};

		u8 drawItemCount = 0;
		u8 vertexBufferViewCount = 0;
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

		const ShaderResourceGroup* const* shaderResourceGroups = nullptr;

		const ShaderResourceGroup* const* uniqueShaderResourceGroups = nullptr;

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
