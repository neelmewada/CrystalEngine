#pragma once

namespace CE::RHI
{
	class GraphicsPipelineCollection;

	/*
	 * Copyright (c) Contributors to the Open 3D Engine Project.
	 * Used under Apache 2.0 license. https://github.com/o3de/o3de/blob/development/LICENSE.TXT
	 */

	enum DrawArgumentsType
	{
		DrawArgumentsIndexed = 0,
		DrawArgumentsLinear = 1,
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

	struct DrawLinearArguments
	{
		u32 instanceCount = 0;
		u32 firstInstance = 0;
		u32 vertexCount = 0;
		u32 vertexOffset = 0;

		inline bool IsValid() const
		{
			return instanceCount > 0 && vertexCount > 0;
		}
	};

	struct DrawArguments
	{
		DrawArguments()
		{}
		
		DrawArguments(const DrawIndexedArguments& indexedArgs)
			: indexedArgs(indexedArgs), type(DrawArgumentsIndexed)
		{}

		DrawArguments(const DrawLinearArguments& linearArgs)
			: linearArgs(linearArgs), type(DrawArgumentsLinear)
		{}

		DrawArgumentsType type = DrawArgumentsIndexed;
		union
		{
			DrawIndexedArguments indexedArgs{};
			DrawLinearArguments linearArgs;
		};
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
		u8 uniqueShaderResourceGroupCount = 0;
		u8 rootConstantSize = 0;
		u8 scissorCount = 0;
		u8 viewportCount = 0;

		/// @brief whether to render this DrawItem or not.
		b8 enabled = true;

		/// @brief The pipeline to use for this draw call.
		RHI::PipelineState* pipelineState = nullptr;

		/// @brief A collection of pipeline variants to use for draw call based on render pipeline.
		RHI::GraphicsPipelineCollection* pipelineCollection = nullptr;

		/// @brief Index buffer to bind for this draw call.
		RHI::IndexBufferView* indexBufferView = nullptr;

		/// @brief Array of vertex buffers to bind for this draw call.
		RHI::VertexBufferView* vertexBufferViews = nullptr;

		/// @brief Array of root constants to bind.
		const u8* rootConstants = nullptr;

		/// @brief Shader resource groups to be bound for this DrawItem.
		RHI::ShaderResourceGroup** shaderResourceGroups = nullptr;

		/// @brief A unique shader resource group that is local to this DrawItem and
		/// is not used outside. Usually a PerDraw SRG.
		RHI::ShaderResourceGroup** uniqueShaderResourceGroups = nullptr;

		/// @brief List of scissor states for this draw call.
		const RHI::ScissorState* scissors = nullptr;

		/// @brief List of viewport states for this draw call.
		const RHI::ViewportState* viewports = nullptr;

	};

	//typedef u32 DrawFilterMask;
	//constexpr DrawFilterMask DrawFilterMaskNullValue = u32(-1);

	enum class DrawFilterMask : u32
	{
		None = 0,
		ALL = UINT_MAX,
	};

	struct DrawItemProperties
	{
		DrawItemProperties() = default;

		DrawItemProperties(const DrawItem* item, const DrawFilterMask filterMask = DrawFilterMask::ALL)
			: item(item), drawFilterMask(filterMask)
		{}

		const DrawItem* item = nullptr;

		inline bool operator==(const DrawItemProperties& props) const
		{
			return item == props.item;
		}

		/// @brief A filter mask which helps decide if this item is supposed to be pushed to a CommandList for drawing.
		DrawFilterMask drawFilterMask = DrawFilterMask::ALL;
	};

} // namespace CE::RPI
