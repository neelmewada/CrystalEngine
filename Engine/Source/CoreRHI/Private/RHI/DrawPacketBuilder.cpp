#include "CoreRHI.h"

namespace CE::RHI
{
	/*
	 * Copyright (c) Contributors to the Open 3D Engine Project.
	 * Used under MIT license. https://github.com/o3de/o3de/blob/development/LICENSE_MIT.TXT
	 */

	DrawPacketBuilder::DrawPacketBuilder(IAllocator* allocator)
	{
		if (!allocator)
			allocator = SystemAllocator::Get();
		this->allocator = allocator;
	}

	void DrawPacketBuilder::SetRootConstants(const u8* rootConstants, u8 rootConstantSize)
	{
		this->rootConstantSize = rootConstantSize;

		for (int i = 0; i < rootConstantSize; i++)
		{
			this->rootConstants[i] = rootConstants[i];
		}
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
		if (drawRequestsCount >= drawRequests.GetSize())
			return;
		if (request.vertexBufferViews.IsEmpty() || request.pipelineState == nullptr)
			return;
		
		vertexBufferViewCount += request.vertexBufferViews.GetSize();
        drawListMask.Set(request.drawItemTag);
		drawRequests[drawRequestsCount++] = request;
	}

	DrawPacket* DrawPacketBuilder::Build()
	{
		if (drawRequestsCount == 0)
			return nullptr;

		SIZE_T drawPacketOffset = AllocateOffset(sizeof(DrawPacket), alignof(DrawPacket));

		SIZE_T drawItemsOffset = AllocateOffset(sizeof(DrawItem) * drawRequestsCount, alignof(DrawItem));

		SIZE_T drawListTagsOffset = AllocateOffset(sizeof(DrawListTag) * drawRequestsCount, alignof(DrawListTag));

		SIZE_T drawFilterMasksOffset = AllocateOffset(sizeof(DrawFilterMask) * drawRequestsCount, alignof(DrawFilterMask));

		SIZE_T rootConstantsOffset = AllocateOffset(sizeof(u8) * rootConstantSize, alignof(u8));

		SIZE_T shaderResourceGroupsOffset = AllocateOffset(sizeof(ShaderResourceGroup*) * shaderResourceGroupCount, alignof(ShaderResourceGroup*));

		SIZE_T uniqueShaderResourceGroupOffset = AllocateOffset(sizeof(ShaderResourceGroup*) * drawRequestsCount, alignof(ShaderResourceGroup*));

		SIZE_T vertexBufferViewsOffset = AllocateOffset(sizeof(VertexBufferView) * vertexBufferViewCount, alignof(VertexBufferView));

		SIZE_T scissorsOffset = AllocateOffset(sizeof(ScissorState) * scissorCount, alignof(ScissorState));

		SIZE_T viewportsOffset = AllocateOffset(sizeof(ViewportState) * viewportCount, alignof(ViewportState));

		SIZE_T totalByteCount = byteOffsetCurrent;

		u8* allocationData = (u8*)allocator->AlignedAlloc(totalByteCount, alignof(DrawPacket));

		auto drawPacket = new(allocationData) DrawPacket();
		drawPacket->allocator = allocator;
        drawPacket->indexBufferView = indexBufferView;
        drawPacket->drawListMask = drawListMask;
        
        if (shaderResourceGroupCount > 0)
        {
            const ShaderResourceGroup** shaderResourceGroups = reinterpret_cast<const ShaderResourceGroup**>(allocationData + shaderResourceGroupsOffset);
            
            for (int i = 0; i < shaderResourceGroupCount; i++) 
            {
                shaderResourceGroups[i] = this->shaderResourceGroups[i];
            }
            
            drawPacket->shaderResourceGroups = shaderResourceGroups;
            drawPacket->shaderResourceGroupCount = shaderResourceGroupCount;
        }
        
        if (drawRequestsCount > 0)
        {
            const ShaderResourceGroup** shaderResourceGroups = reinterpret_cast<const ShaderResourceGroup**>(allocationData + shaderResourceGroupsOffset);
            
            for (int i = 0; i < drawRequestsCount; i++)
            {
                shaderResourceGroups[i] = this->drawRequests[i].uniqueShaderResourceGroup;
            }
			
            drawPacket->uniqueShaderResourceGroups = shaderResourceGroups;
            drawPacket->uniqueShaderResourceGroupCount = drawRequestsCount;
        }
        
        if (scissorCount > 0)
        {
            ScissorState* scissors = reinterpret_cast<ScissorState*>(allocationData + scissorsOffset);
            
            for (int i = 0; i < scissorCount; i++)
            {
                scissors[i] = this->scissors[i];
            }
            
            drawPacket->scissors = scissors;
            drawPacket->scissorCount = scissorCount;
        }
        
        if (viewportCount > 0)
        {
            ViewportState* viewports = reinterpret_cast<ViewportState*>(allocationData + viewportsOffset);
            
            for (int i = 0; i < viewportCount; i++)
            {
                viewports[i] = this->viewports[i];
            }
            
            drawPacket->viewports = viewports;
            drawPacket->viewportCount = viewportCount;
        }

		if (rootConstantSize > 0)
		{
			auto rootConstants = reinterpret_cast<u8*>(allocationData + rootConstantsOffset);
			memcpy(rootConstants, this->rootConstants.GetData(), rootConstantSize);
			
			drawPacket->rootConstants = rootConstants;
			drawPacket->rootConstantSize = rootConstantSize;
		}
        
        auto drawItems = reinterpret_cast<DrawItem*>(allocationData + drawItemsOffset);
        auto drawListTags = reinterpret_cast<DrawListTag*>(allocationData + drawListTagsOffset);
        auto drawFilterMasks = reinterpret_cast<DrawFilterMask*>(allocationData + drawFilterMasksOffset);
        
        drawPacket->drawItems = drawItems;
        drawPacket->drawListTags = drawListTags;
        drawPacket->drawFilterMasks = drawFilterMasks;
        
        for (int i = 0; i < drawRequestsCount; i++)
        {
            const DrawItemRequest& drawRequest = this->drawRequests[i];
            
            drawListTags[i] = drawRequest.drawItemTag;
            drawFilterMasks[i] = drawRequest.drawFilterMask;
            
            DrawItem& drawItem = drawItems[i];
            drawItem.enabled = true; // TODO: Add enabled flag
            drawItem.indexBufferView = &drawPacket->indexBufferView;
            drawItem.vertexBufferViewCount = 0;
			drawItem.vertexBufferViews = nullptr;
            drawItem.scissorCount = drawPacket->scissorCount;
			drawItem.scissors = drawPacket->scissors;
            drawItem.viewportCount = drawPacket->viewportCount;
            drawItem.viewports = drawPacket->viewports;
            drawItem.shaderResourceGroups = drawPacket->shaderResourceGroups;
            drawItem.shaderResourceGroupCount = drawPacket->shaderResourceGroupCount;
            drawItem.uniqueShaderResourceGroup = drawPacket->uniqueShaderResourceGroups[i];
            drawItem.stencilRef = drawRequest.stencilRef;
            drawItem.pipelineState = drawRequest.pipelineState;
            drawItem.arguments = drawArguments;
			drawItem.rootConstants = drawPacket->rootConstants;
			drawItem.rootConstantSize = drawPacket->rootConstantSize;
        }

		if (vertexBufferViewCount > 0)
		{
			auto vertexBufferViews = reinterpret_cast<VertexBufferView*>(allocationData + vertexBufferViewsOffset);

			drawPacket->vertexBufferViews = vertexBufferViews;
			drawPacket->vertexBufferViewCount = vertexBufferViewCount;

			for (int i = 0; i < drawRequestsCount; i++)
			{
				const DrawItemRequest& drawRequest = this->drawRequests[i];

				if (!drawRequest.vertexBufferViews.IsEmpty())
				{
					DrawItem& drawItem = drawItems[i];
					drawItem.vertexBufferViews = vertexBufferViews;
					drawItem.vertexBufferViewCount = vertexBufferViewCount;

					for (const VertexBufferView& vertexBufferView : drawRequest.vertexBufferViews)
					{
						*vertexBufferViews++ = vertexBufferView;
					}
				}
			}
		}
        
		return drawPacket;
	}

	SIZE_T DrawPacketBuilder::AllocateOffset(SIZE_T byteCount, SIZE_T byteAlignment)
	{
        if (byteCount == 0)
            return byteOffsetCurrent;
        
		SIZE_T addressCurrentAligned = Memory::GetAlignedSize(byteOffsetCurrent, byteAlignment);
		SIZE_T byteCountAligned = Memory::GetAlignedSize(byteCount, byteAlignment);
		SIZE_T nextByteAddress = addressCurrentAligned + byteCountAligned;

		byteOffsetCurrent = nextByteAddress;
		return addressCurrentAligned;
	}

} // namespace CE::RHI