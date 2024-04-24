#pragma once

namespace CE::RHI
{
	/*
	 * Copyright (c) Contributors to the Open 3D Engine Project.
	 * Used under MIT license. https://github.com/o3de/o3de/blob/development/LICENSE_MIT.TXT
	 */

	class CORERHI_API DrawPacketBuilder final
	{
	public:
		static constexpr u32 DrawItemCountMax = 32;

		DrawPacketBuilder(IAllocator* allocator = nullptr);

		struct DrawItemRequest
		{
			u8 stencilRef = 0;

			DrawListTag drawItemTag;

			IndexBufferView indexBufferView{};

			Array<VertexBufferView> vertexBufferViews{};

			Array<ShaderResourceGroup*> uniqueShaderResourceGroups{};

			PipelineState* pipelineState = nullptr;

			GraphicsPipelineCollection* pipelineCollection = nullptr;

			DrawFilterMask drawFilterMask = DrawFilterMask::ALL;
		};

		void SetRootConstants(const u8* rootConstants, u8 rootConstantSize);

		void SetDrawArguments(const DrawArguments& drawArguments);

		void AddScissorState(const ScissorState& scissorState);
		void AddViewportState(const ViewportState& viewportState);

		void AddShaderResourceGroup(ShaderResourceGroup* srg);

		void AddDrawItem(const DrawItemRequest& request);

		DrawPacket* Build();

	private:

		SIZE_T AllocateOffset(SIZE_T byteCount, SIZE_T byteAlignment);

		IAllocator* allocator = nullptr;

		DrawArguments drawArguments;
        DrawListMask drawListMask{};

		FixedArray<ScissorState, Limits::Pipeline::MaxColorAttachmentCount> scissors{};
		FixedArray<ViewportState, Limits::Pipeline::MaxColorAttachmentCount> viewports{};
		StaticArray<ShaderResourceGroup*, Limits::Pipeline::MaxShaderResourceGroupCount> shaderResourceGroups{};
		u32 shaderResourceGroupCount = 0;
		u32 uniqueShaderResourceGroupCount = 0;
		FixedArray<DrawItemRequest, DrawItemCountMax> drawRequests{};
		u32 vertexBufferViewCount = 0;
		StaticArray<u8, Limits::Pipeline::MaxRootConstantSize> rootConstants{};
		u8 rootConstantSize = 0;


		SIZE_T byteOffsetCurrent = 0;
	};
    
} // namespace CE::RPI
