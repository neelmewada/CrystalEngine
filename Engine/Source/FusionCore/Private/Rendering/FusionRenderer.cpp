#include "FusionCore.h"

namespace CE
{
	constexpr bool ForceDisableBatching = false;

	FusionRenderer::FusionRenderer()
	{
		
	}

	void FusionRenderer::Init(const FusionRendererInitInfo& initInfo)
	{
		fusionShader = initInfo.fusionShader;
		multisampling = initInfo.multisampling;

		numFrames = RHI::FrameScheduler::Get()->GetFramesInFlight();

		drawItemsBuffer.Init("DrawItems_" + GetName().GetString(), initialDrawItemCapacity, numFrames);

		drawItemSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(FusionApplication::Get()->perDrawSrgLayout);

		perViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(FusionApplication::Get()->perViewSrgLayout);

		for (int i = 0; i < numFrames; ++i)
		{
			RHI::BufferDescriptor viewConstantsBufferDesc{};
			viewConstantsBufferDesc.name = "ViewConstants";
			viewConstantsBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
			viewConstantsBufferDesc.bufferSize = sizeof(RPI::PerViewConstants);
			viewConstantsBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
			viewConstantsBufferDesc.structureByteStride = sizeof(RPI::PerViewConstants);

			viewConstantsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(viewConstantsBufferDesc);
			viewConstantsUpdateRequired[i] = true;

			perViewSrg->Bind(i, "_PerViewData", viewConstantsBuffer[i]);
			drawItemSrg->Bind(i, "_DrawList", drawItemsBuffer.GetBuffer(i));
		}

		perViewSrg->FlushBindings();
		drawItemSrg->FlushBindings();
	}

	void FusionRenderer::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		drawItemsBuffer.Shutdown();

		for (int i = 0; i < numFrames; i++)
		{
			delete viewConstantsBuffer[i];
			viewConstantsBuffer[i] = nullptr;
		}

		for (auto drawPacket : drawPackets)
		{
			delete drawPacket;
		}
		drawPackets.Clear();

		delete perViewSrg; perViewSrg = nullptr;
		delete drawItemSrg; drawItemSrg = nullptr;
	}

	void FusionRenderer::Begin()
	{
		ZoneScoped;

		drawBatches.Clear();

		drawItemCount = 0;
		createNewDrawBatch = true;

		for (int i = 0; i < resubmitDrawItems.GetSize(); ++i)
		{
			resubmitDrawItems[i] = true;
		}
	}

	void FusionRenderer::End()
	{
		ZoneScoped;

		{
			FDrawItem2D drawItem;
			drawItem.transform = rootTransform * Matrix4x4::Translation(Vec3(1024 * 0.25f, 768 * 0.25f, 0)) * Matrix4x4::Scale(Vec3(1024.0f * 0.5f, 768 * 0.5f));
			drawItem.drawType = DRAW_Rect;

			drawItemList.Resize(1);
			drawItemList[0] = drawItem;

			drawBatches.Add(FDrawBatch{ .firstDrawItemIndex = 0, .drawItemCount = 1 });
			drawItemCount++;
		}

		const auto& vertBuffers = RPI::RPISystem::Get().GetTextQuad();
		RHI::DrawLinearArguments drawArgs = RPI::RPISystem::Get().GetTextQuadDrawArgs();

		RHI::DrawPacket* drawPacket = nullptr;

		Array<RHI::DrawPacket*> oldPackets = this->drawPackets;
		this->drawPackets.Clear();

		for (int i = 0; i < drawBatches.GetSize(); ++i)
		{
			const FDrawBatch& drawBatch = drawBatches[i];

			if (oldPackets.NonEmpty()) // Reuse draw packet
			{
				drawPacket = oldPackets[0];
				oldPackets.RemoveAt(0);

				drawPacket->drawItems[0].arguments.linearArgs.firstInstance = drawBatch.firstDrawItemIndex;
				drawPacket->drawItems[0].arguments.linearArgs.instanceCount = drawBatch.drawItemCount;

				drawPacket->shaderResourceGroups[0] = drawItemSrg;
				drawPacket->shaderResourceGroups[1] = perViewSrg;

				this->drawPackets.Add(drawPacket);
			}
			else // Create new draw packet
			{
				RHI::DrawPacketBuilder builder{};

				drawArgs.firstInstance = drawBatch.firstDrawItemIndex;
				drawArgs.instanceCount = drawBatch.drawItemCount;
				builder.SetDrawArguments(drawArgs);

				builder.AddShaderResourceGroup(drawItemSrg);
				builder.AddShaderResourceGroup(perViewSrg);

				// UI Item
				{
					RHI::DrawPacketBuilder::DrawItemRequest request{};
					request.vertexBufferViews.AddRange(vertBuffers);

					request.drawItemTag = drawListTag;
					request.drawFilterMask = RHI::DrawFilterMask::ALL;

					request.pipelineState = fusionShader->GetVariant(0)->GetPipeline(multisampling);

					builder.AddDrawItem(request);
				}

				drawPacket = builder.Build();
				this->drawPackets.Add(drawPacket);
			}
		}
	}

	const Array<RHI::DrawPacket*>& FusionRenderer::FlushDrawPackets(u32 imageIndex)
	{
		ZoneScoped;

		curImageIndex = (int)imageIndex;

		drawItemGrowRatio = Math::Clamp01(drawItemGrowRatio);

		if (viewConstantsUpdateRequired[imageIndex])
		{
			viewConstantsBuffer[imageIndex]->UploadData(&viewConstants, sizeof(viewConstants));

			viewConstantsUpdateRequired[imageIndex] = false;
		}

		if (resubmitDrawItems[imageIndex])
		{
			if (drawItemCount > 0)
			{
				if (drawItemsBuffer.GetElementCount() < drawItemCount)
				{
					u64 growCount = (u64)((f32)drawItemsBuffer.GetElementCount() * (1.0f + drawItemGrowRatio));
					drawItemsBuffer.GrowToFit(Math::Max<u64>(drawItemCount + 50, growCount));
				}

				drawItemsBuffer.GetBuffer(imageIndex)->UploadData(drawItemList.GetData(), drawItemCount * sizeof(FDrawItem2D));
			}

			resubmitDrawItems[imageIndex] = false;
		}

		return drawPackets;
	}

	void FusionRenderer::SetViewConstants(const RPI::PerViewConstants& viewConstants)
	{
		this->viewConstants = viewConstants;

		for (int i = 0; i < viewConstantsUpdateRequired.GetSize(); ++i)
		{
			viewConstantsUpdateRequired[i] = true;
		}
	}

	void FusionRenderer::SetRootTransform(const Matrix4x4& transform)
	{
		rootTransform = transform;
	}

	void FusionRenderer::SetScreenSize(Vec2i screenSize)
	{
		this->screenSize = screenSize;
	}

	void FusionRenderer::SetDrawListTag(RHI::DrawListTag drawListTag)
	{
		this->drawListTag = drawListTag;
	}

} // namespace CE
