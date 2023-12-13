#pragma once

namespace CE::RHI
{

	class CORERHI_API DrawPacketBuilder final
	{
	public:
		static constexpr u32 DrawItemCountMax = 16;

		DrawPacketBuilder(IAllocator* allocator = nullptr);

		struct DrawItemRequest
		{
			u8 stencilRef = 0;

			DrawListTag drawItemTag;

			Array<VertexBufferView> vertexBufferViews{};

			ShaderResourceGroup* uniqueShaderResourceGroup = nullptr;

			IPipelineState* pipelineState = nullptr;

			DrawFilterMask drawFilterMask = DrawFilterMaskNullValue;
		};

		void SetDrawArguments(const DrawArguments& drawArguments);

		void SetIndexBufferView(const IndexBufferView& indexBufferView);

		void AddScissorState(const ScissorState& scissorState);
		void AddViewportState(const ViewportState& viewportState);

		void AddShaderResourceGroup(ShaderResourceGroup* srg);

		void AddDrawItem(const DrawItemRequest& request);

		DrawPacket* Build();

	private:

		SIZE_T AllocateOffset(SIZE_T byteCount, SIZE_T byteAlignment);

		IAllocator* allocator = nullptr;

		DrawArguments drawArguments;
		IndexBufferView indexBufferView{};

		FixedArray<ScissorState, Limits::Pipeline::MaxColorAttachmentCount> scissors{};
		u32 scissorCount = 0;
		FixedArray<ViewportState, Limits::Pipeline::MaxColorAttachmentCount> viewports{};
		u32 viewportCount = 0;
		FixedArray<ShaderResourceGroup*, Limits::Pipeline::MaxShaderResourceGroupCount> shaderResourceGroups{};
		u32 shaderResourceGroupCount = 0;
		FixedArray<DrawItemRequest, DrawItemCountMax> drawItems{};
		u32 drawItemCount = 0;
		u32 vertexBufferViewCount = 0;

		SIZE_T byteOffsetCurrent = 0;
	};
    
} // namespace CE::RPI
