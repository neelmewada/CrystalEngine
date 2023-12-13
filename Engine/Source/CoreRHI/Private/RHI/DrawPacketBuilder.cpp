#include "CoreRHI.h"

namespace CE::RHI
{
	DrawPacketBuilder::DrawPacketBuilder(IAllocator* allocator)
	{
		if (!allocator)
			allocator = SystemAllocator::Get();
		this->allocator = allocator;
	}

	void DrawPacketBuilder::SetDrawArguments(const DrawArguments& args)
	{
		this->drawArguments = args;
	}

	void DrawPacketBuilder::SetIndexBufferView(const IndexBufferView& indexBufferView)
	{
		this->indexBufferView = indexBufferView;
	}

	void DrawPacketBuilder::AddScissorState(const ScissorState& scissorState)
	{
		if (scissorCount >= scissors.GetSize())
			return;

		scissors[scissorCount++] = scissorState;
	}

	void DrawPacketBuilder::AddViewportState(const ViewportState& viewportState)
	{
		if (viewportCount >= viewports.GetSize())
			return;

		viewports[viewportCount++] = viewportState;
	}

	void DrawPacketBuilder::AddShaderResourceGroup(ShaderResourceGroup* srg)
	{
		if (shaderResourceGroupCount >= shaderResourceGroups.GetSize())
			return;

		for (int i = 0; i < shaderResourceGroupCount; i++)
		{
			if (shaderResourceGroups[i] == srg)
				return;
		}

		shaderResourceGroups[shaderResourceGroupCount++] = srg;
	}

	void DrawPacketBuilder::AddDrawItem(const DrawItemRequest& request)
	{
		if (drawItemCount >= drawItems.GetSize())
			return;
		if (request.vertexBufferViews.IsEmpty() || request.pipelineState == nullptr)
			return;
		
		vertexBufferViewCount += request.vertexBufferViews.GetSize();
		drawItems[drawItemCount++] = request;
	}

	DrawPacket* DrawPacketBuilder::Build()
	{
		if (drawItemCount == 0)
			return nullptr;

		SIZE_T drawPacketOffset = AllocateOffset(sizeof(DrawPacket), alignof(DrawPacket));

		SIZE_T drawItemsOffset = AllocateOffset(sizeof(DrawItem) * drawItemCount, alignof(DrawItem));

		SIZE_T drawListTagsOffset = AllocateOffset(sizeof(DrawListTag) * drawItemCount, alignof(DrawListTag));

		SIZE_T drawFilterMasksOffset = AllocateOffset(sizeof(DrawFilterMask) * drawItemCount, alignof(DrawFilterMask));

		SIZE_T shaderResourceGroupsOffset = AllocateOffset(sizeof(ShaderResourceGroup*) * shaderResourceGroupCount, alignof(ShaderResourceGroup*));

		SIZE_T uniqueShaderResourceGroupOffset = AllocateOffset(sizeof(ShaderResourceGroup*) * drawItemCount, alignof(ShaderResourceGroup*));

		SIZE_T vertexBufferViewsOffset = AllocateOffset(sizeof(VertexBufferView) * vertexBufferViewCount, alignof(VertexBufferView));

		SIZE_T scissorsOffset = AllocateOffset(sizeof(ScissorState) * scissorCount, alignof(ScissorState));

		SIZE_T viewportsOffset = AllocateOffset(sizeof(ViewportState) * viewportCount, alignof(ViewportState));

		SIZE_T totalByteCount = vertexBufferViewCount;

		u8* allocationData = (u8*)allocator->AlignedAlloc(totalByteCount, alignof(DrawPacket));

		auto drawPacket = new(allocationData) DrawPacket();
		drawPacket->allocator = allocator;

		return drawPacket;
	}

	SIZE_T DrawPacketBuilder::AllocateOffset(SIZE_T byteCount, SIZE_T byteAlignment)
	{
		SIZE_T addressCurrentAligned = Memory::GetAlignedSize(byteOffsetCurrent, byteAlignment);
		SIZE_T byteCountAligned = Memory::GetAlignedSize(byteCount, byteAlignment);
		SIZE_T nextByteAddress = addressCurrentAligned + byteCountAligned;

		byteOffsetCurrent = nextByteAddress;
		return addressCurrentAligned;
	}

} // namespace CE::RHI
