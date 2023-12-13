#pragma once

namespace CE::RHI
{
	enum DrawArgumentsType
	{
		DrawArgumentsIndexed = 0,
	};

	struct DrawIndexedArguments
	{
		u32 instanceCount = 0;
		u32 firstInstance = 0;
		u32 vertexOffset = 0;
		u32 indexCount = 0;
		u32 firstIndex = 0;

		inline bool IsValid() const
		{
			return instanceCount > 0 && indexCount > 0;
		}
	};

	struct DrawArguments
	{
		DrawArguments()
		{}
		
		DrawArguments(const DrawIndexedArguments& indexedArgs)
			: indexedArgs(indexedArgs), type(DrawArgumentsIndexed)
		{}

		DrawArgumentsType type = DrawArgumentsIndexed;
		DrawIndexedArguments indexedArgs{};
	};

	/// @brief A DrawItem is a draw call of one mesh in one pass. Multiple DrawItems are
	/// bundled together in a single DrawPacket, which corresponds to multiple draw calls
	/// of one mesh in multiple passes.
	struct DrawItem
	{
	public:

		DrawArguments arguments{};

		u8 stencilRef = 0;
		u8 vertexBufferViewCount = 0;
		u8 shaderResourceGroupCount = 0;
		u8 scissorCount = 0;
		u8 viewportCount = 0;

		/// @brief whether to render this DrawItem or not.
		b8 enabled = true;

		/// @brief The pipeline to use for this draw call.
		RHI::IPipelineState* pipelineState = nullptr;

		/// @brief Index buffer to bind for this draw call.
		RHI::IndexBufferView* indexBufferView = nullptr;

		/// @brief Array of vertex buffers to bind for this draw call.
		RHI::VertexBufferView* vertexBufferViews = nullptr;

		/// @brief Shader resource groups to be bound for this DrawItem.
		RHI::ShaderResourceGroup* shaderResourceGroups = nullptr;

		/// @brief A unique shader resource group that is local to this DrawItem and
		/// is not used outside. Usually a PerDraw SRG.
		RHI::ShaderResourceGroup* uniqueShaderResourceGroup = nullptr;

		RHI::ScissorState* scissors = nullptr;

		RHI::ViewportState* viewports = nullptr;

	};

	typedef u32 DrawFilterMask;
	constexpr u32 DrawFilterMaskNullValue = u32(-1);

	struct DrawItemProperties
	{
		DrawItemProperties() = default;

		DrawItemProperties(const DrawItem* item, const DrawFilterMask filterMask = DrawFilterMaskNullValue)
			: item(item), drawFilterMask(filterMask)
		{}

		const DrawItem* item = nullptr;

		/// @brief A filter mask which helps decide if this item is supposed to be pushed to a CommandList for drawing.
		DrawFilterMask drawFilterMask = DrawFilterMaskNullValue;
	};

} // namespace CE::RPI
