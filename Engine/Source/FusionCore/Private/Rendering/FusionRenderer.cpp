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
		drawItemList.RemoveAll();

		createNewDrawBatch = true;

		for (int i = 0; i < resubmitDrawItems.GetSize(); ++i)
		{
			resubmitDrawItems[i] = true;
		}
	}

	void FusionRenderer::End()
	{
		ZoneScoped;
		
		if (true)
		{
			angleDegrees = 0;
			scaling = Vec2(1, 1);
			transformOverlay = Matrix4x4::Translation(Vec3(0, 0, 0)) * 
				Quat::EulerDegrees(0, 0, 23.5f).ToMatrix() *
				Matrix4x4::Scale(Vec3(0.75f, 0.75f, 1));

			DrawCustomItem(DRAW_Shape, Vec2(1024 * 0.f, 768 * 0.f), Vec2(1024.0f * 0.5f, 768 * 0.5f));
			DrawCustomItem(DRAW_Shape, Vec2(1024 * 0.5f, 768 * 0.5f), Vec2(1024.0f * 0.5f, 768 * 0.5f));
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

		// Cleanup unused draw packets

		while (oldPackets.NonEmpty())
		{
			delete oldPackets[0];
			oldPackets.RemoveAt(0);
		}
	}

	///////////////////////////////////////////////
	// - Draw API -

	FusionRenderer::FDrawItem2D& FusionRenderer::DrawCustomItem(FDrawType drawType, Vec2 pos, Vec2 size)
	{
		ZoneScoped;

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
			drawBatches.Add({});
			drawBatches.Top().firstDrawItemIndex = drawItemList.GetCount();
		}

		FDrawItem2D drawItem{};
		drawItem.drawType = drawType;

		Vec3 translation1 = Vec3(-size.x / 2, -size.y / 2, 0);
		Vec3 translation2 = Vec3(pos.x + size.x / 2, pos.y + size.y / 2, 0);
		
		drawItem.transform = rootTransform *
			Matrix4x4::Translation(translation2) *
			transformOverlay *
			Matrix4x4::Translation(translation1) *
			Matrix4x4::Scale(Vec3(size.x, size.y, 1));
		
		drawItemList.Insert(drawItem);

		drawBatches.Top().drawItemCount++;

		return drawItemList.Last();
	}

	///////////////////////////////////////////////
	// - Draw Packets -

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
			if (drawItemList.GetCount() > 0)
			{
				if (drawItemsBuffer.GetElementCount() < drawItemList.GetCount())
				{
					u64 totalCount = (u64)((f32)drawItemsBuffer.GetElementCount() * (1.0f + drawItemGrowRatio));
					drawItemsBuffer.GrowToFit(Math::Max<u64>(drawItemList.GetCount() + 50, totalCount));
				}

				drawItemsBuffer.GetBuffer(imageIndex)->UploadData(drawItemList.GetData(), drawItemList.GetCount() * sizeof(FDrawItem2D));
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

	void FusionRenderer::SetTransformOverlay(const Matrix4x4& transform)
	{
		this->transformOverlay = transform;
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
