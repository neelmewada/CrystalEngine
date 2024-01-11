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

		void SetRootConstants(const u8* rootConstants, u8 rootConstantSize);

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
        DrawListMask drawListMask{};

		FixedArray<ScissorState, Limits::Pipeline::MaxBoundAttachmentCount> scissors{};
		FixedArray<ViewportState, Limits::Pipeline::MaxBoundAttachmentCount> viewports{};
		StaticArray<ShaderResourceGroup*, Limits::Pipeline::MaxShaderResourceGroupCount> shaderResourceGroups{};
		u32 shaderResourceGroupCount = 0;
		StaticArray<DrawItemRequest, DrawItemCountMax> drawRequests{};
		u32 drawRequestsCount = 0;
		u32 vertexBufferViewCount = 0;
		StaticArray<u8, Limits::Pipeline::MaxRootConstantSize> rootConstants{};
		u8 rootConstantSize = 0;


		SIZE_T byteOffsetCurrent = 0;
	};
    
} // namespace CE::RPI
