#pragma once

namespace CE::RPI
{
	struct DrawIndexedArguments
	{
		u32 instanceCount = 0;
		u32 firstInstance = 0;
		u32 vertexOffset = 0;
		u32 indexCount = 0;
		u32 firstIndex = 0;
	};

	/// @brief A DrawItem is a draw call of one mesh in one pass. Multiple DrawItems are
	/// bundled together in a single DrawPacket, which corresponds to multiple draw calls
	/// of one mesh in multiple passes.
	struct DrawItem
	{
	public:

		DrawIndexedArguments arguments{};

		u8 stencilRef = 0;
		u8 vertexBufferViewCount = 0;
		u8 shaderResourceGroupCount = 0;
		u8 scissorRectCount = 0;
		u8 viewportRectCount = 0;

		/// @brief whether to render this DrawItem or not
		b8 enabled = true;

		RHI::IPipelineState* pipelineState = nullptr;

		RHI::IndexBufferView* indexBufferView = nullptr;

		RHI::VertexBufferView* vertexBufferViews = nullptr;

		/// @brief Shader resource groups to be bound for this DrawItem. 
		RHI::ShaderResourceGroup* shaderResourceGroups = nullptr;

		/// @brief A unique shader resource group that is local to this DrawItem and is not shared outside. Usually a PerDraw SRG.
		RHI::ShaderResourceGroup* uniqueShaderResourceGroup = nullptr;

		RHI::ScissorRect* scissorRects = nullptr;

		RHI::ViewportRect* viewportRects = nullptr;
	};

} // namespace CE::RPI
