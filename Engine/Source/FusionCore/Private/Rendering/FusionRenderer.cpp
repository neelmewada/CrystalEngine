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
		clipItemsBuffer.Init("ClipItems_" + GetName().GetString(), initialClipItemCapacity, numFrames);
		shapeItemsBuffer.Init("ShapeItems_" + GetName().GetString(), initialShapeItemCapacity, numFrames);

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
			drawItemSrg->Bind(i, "_ClipItems", clipItemsBuffer.GetBuffer(i));
			drawItemSrg->Bind(i, "_ShapeDrawList", shapeItemsBuffer.GetBuffer(i));
		}

		perViewSrg->FlushBindings();
		drawItemSrg->FlushBindings();
	}

	void FusionRenderer::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		drawItemsBuffer.Shutdown();
		clipItemsBuffer.Shutdown();
		shapeItemsBuffer.Shutdown();

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

		FFontManager* fontManager = FusionApplication::Get()->fontManager;

		currentBrush = FBrush();
		currentPen = FPen();
		currentFont = FFont(fontManager->GetDefaultFontFamily(), 13);
		itemTransform = Matrix4x4::Identity();

		drawBatches.Clear();
		drawItemList.RemoveAll();
		clipItemList.RemoveAll();
		clipItemStack.RemoveAll();
		shapeItemList.RemoveAll();
		coordinateSpaceStack.RemoveAll();

		createNewDrawBatch = true;

		for (int i = 0; i < resubmitDrawItems.GetSize(); ++i)
		{
			resubmitDrawItems[i] = true;
		}

		PushChildCoordinateSpace(Matrix4x4::Identity());
	}

	void FusionRenderer::End()
	{
		ZoneScoped;

		PopChildCoordinateSpace(); // Pop identity matrix

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
				drawPacket->shaderResourceGroups[2] = drawBatch.fontSrg;

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
				builder.AddShaderResourceGroup(drawBatch.fontSrg);

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

		FFontManager* fontManager = FusionApplication::Get()->fontManager;

		Name fontFamily = currentFont.GetFamily();
		int fontSize = currentFont.GetFontSize();

		if (fontSize <= 0)
			fontSize = 12;
		if (!fontFamily.IsValid())
			fontFamily = fontManager->GetDefaultFontFamily();

		FFontAtlas* fontAtlas = fontManager->FindFont(fontFamily);

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
			drawBatches.Add({});
			drawBatches.Top().firstDrawItemIndex = drawItemList.GetCount();
			drawBatches.Top().fontSrg = fontAtlas != nullptr ? fontAtlas->fontSrg : nullptr;
		}

		FDrawItem2D drawItem{};
		drawItem.drawType = drawType;
		drawItem.quadSize = size;
		drawItem.opacity = opacityStack.IsEmpty() ? 1.0f : opacityStack.Last();

		switch (currentPen.GetStyle())
		{
		case FPenStyle::None:
			break;
		case FPenStyle::SolidLine:
			drawItem.penColor = currentPen.GetColor().ToVec4();
			drawItem.penThickness = currentPen.GetThickness();
			break;
		}

		// Translate to quad's center point first -> then apply item transform -> then translate back to top-left point of quad
		Vec3 translation1 = Vec3(-size.x / 2, -size.y / 2, 0);
		Vec3 translation2 = Vec3(pos.x + size.x / 2, pos.y + size.y / 2, 0);
		
		drawItem.transform = coordinateSpaceStack.Last() *
			Matrix4x4::Translation(translation2) *
			itemTransform *
			Matrix4x4::Translation(translation1) *
			Matrix4x4::Scale(Vec3(size.x, size.y, 1));

		drawItem.clipIndex = -1;
		if (!clipItemStack.IsEmpty())
		{
			drawItem.clipIndex = clipItemStack.Last();
		}
		
		drawItemList.Insert(drawItem);

		drawBatches.Top().drawItemCount++;

		return drawItemList.Last();
	}

	Vec2 FusionRenderer::DrawText(const String& text, Vec2 pos, Vec2 size)
	{
		if (text.IsEmpty())
			return Vec2();

		FFontManager* fontManager = FusionApplication::Get()->fontManager;

		Name fontFamily = currentFont.GetFamily();
		int fontSize = currentFont.GetFontSize();

		if (fontSize <= 0)
			fontSize = 12;
		if (!fontFamily.IsValid())
			fontFamily = fontManager->GetDefaultFontFamily();

		fontSize = Math::Max(fontSize, 6);

		const bool isFixedWidth = size.x > 0;
		const bool isFixedHeight = size.y > 0;

		FFontAtlas* fontAtlas = fontManager->FindFont(fontFamily);
		if (fontAtlas == nullptr)
			return Vec2();

		const FFontMetrics& metrics = fontAtlas->GetMetrics();

		const float startY = pos.y + metrics.ascender * (f32)fontSize;
		const float startX = pos.x;

		float maxX = startX;
		float maxY = startY;

		Vec3 curPos = Vec3(startX, startY, 0);

		int totalCharactersDrawn = 0;
		int firstDrawItemIndex = drawItemList.GetCount();

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
			drawBatches.Add({});
			drawBatches.Top().firstDrawItemIndex = drawItemList.GetCount();
			drawBatches.Top().fontSrg = fontAtlas->fontSrg;
		}

		Vec2 finalSize;

		int whitespaceIdx = -1;
		int idx = 0;

		for (int i = 0; i < text.GetLength(); ++i)
		{
			char c = text[i];

			if (c == ' ')
			{
				whitespaceIdx = i;
			}

			if (c == '\n')
			{
				whitespaceIdx = -1;
				curPos.x = startX;
				curPos.y += metrics.lineHeight * fontSize;
				continue;
			}

			drawItemList.Insert(FDrawItem2D());

			FDrawItem2D& drawItem = drawItemList.Last();
			drawItem.drawType = DRAW_Text;
			drawItem.opacity = opacityStack.IsEmpty() ? 1.0f : opacityStack.Last();

			switch (currentPen.GetStyle())
			{
			case FPenStyle::None:
				break;
			case FPenStyle::SolidLine:
				drawItem.penColor = currentPen.GetColor().ToVec4();
				drawItem.penThickness = currentPen.GetThickness();
				break;
			}

			drawItem.clipIndex = -1;
			if (!clipItemStack.IsEmpty())
			{
				drawItem.clipIndex = clipItemStack.Last();
			}

			FFontGlyphInfo glyph = fontAtlas->FindOrAddGlyph(c, fontSize);

			drawItem.charIndex = glyph.index;

			const float glyphWidth = (f32)glyph.GetWidth() * (f32)fontSize / (f32)glyph.fontSize;
			const float glyphHeight = (f32)glyph.GetHeight() * (f32)fontSize / (f32)glyph.fontSize;

			drawItem.quadSize = Vec2(glyphWidth, glyphHeight);

			curPos.x += (f32)glyph.xOffset * (f32)fontSize / (f32)glyph.fontSize;

			Vec2 quadPos = curPos;
			quadPos.y -= (f32)glyph.yOffset * (f32)fontSize / (f32)glyph.fontSize;

			drawItem.transform = coordinateSpaceStack.Last() *
				Matrix4x4::Translation(Vec3(quadPos.x, quadPos.y)) *
				Matrix4x4::Scale(Vec3(drawItem.quadSize.width, drawItem.quadSize.height, 1));

			curPos.x += (f32)glyph.advance * (f32)fontSize / (f32)glyph.fontSize - (f32)glyph.xOffset * (f32)fontSize / (f32)glyph.fontSize;

			if (curPos.x > maxX)
				maxX = curPos.x;
			if (curPos.y + metrics.lineHeight * (f32)fontSize / (f32)glyph.fontSize > maxY)
				maxY = curPos.y + metrics.lineHeight * (f32)fontSize / (f32)glyph.fontSize;

			totalCharactersDrawn++;
			idx++;
		}

		drawBatches.Top().drawItemCount += totalCharactersDrawn;

		return finalSize;
	}

	FusionRenderer::FDrawItem2D& FusionRenderer::DrawShape(const FShape& shape, Vec2 pos, Vec2 size)
	{
		ZoneScoped;

		FDrawItem2D& drawItem = DrawCustomItem(DRAW_Shape, pos, size);

		FShapeItem2D shapeItem;
		shapeItem.shape = shape.GetShapeType();
		shapeItem.cornerRadius = shape.GetCornerRadius();

		switch (currentBrush.GetBrushStyle())
		{
		case FBrushStyle::None:
			break;
		case FBrushStyle::SolidFill:
			shapeItem.brushType = BRUSH_Solid;
			break;
		case FBrushStyle::TexturePattern:
			shapeItem.brushType = BRUSH_Texture;
			break;
		case FBrushStyle::LinearGradient:
			shapeItem.brushType = BRUSH_LinearGradient;
			break;
		}

		switch (currentPen.GetStyle())
		{
		case FPenStyle::None:
			break;
		case FPenStyle::SolidLine:
			shapeItem.penType = PEN_SolidLine;
			break;
		}

		shapeItem.brushColor = currentBrush.GetFillColor().ToVec4();

		drawItem.shapeIndex = shapeItemList.GetCount();

		shapeItemList.Insert(shapeItem);

		return drawItem;
	}

	///////////////////////////////////////////////
	// - Draw Packets -

	const Array<RHI::DrawPacket*>& FusionRenderer::FlushDrawPackets(u32 imageIndex)
	{
		ZoneScoped;

		curImageIndex = (int)imageIndex;

		drawItemGrowRatio = Math::Clamp01(drawItemGrowRatio);
		clipItemGrowRatio = Math::Clamp01(clipItemGrowRatio);
		shapeItemGrowRatio = Math::Clamp01(shapeItemGrowRatio);

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
					drawItemsBuffer.GrowToFit(Math::Max<u64>(drawItemList.GetCount() + 64, totalCount));
				}

				drawItemsBuffer.GetBuffer(imageIndex)->UploadData(drawItemList.GetData(), drawItemList.GetCount() * sizeof(FDrawItem2D));
			}

			if (clipItemList.GetCount() > 0)
			{
				if (clipItemsBuffer.GetElementCount() < clipItemList.GetCount())
				{
					u64 totalCount = (u64)((f32)clipItemsBuffer.GetElementCount() * (1.0f + clipItemGrowRatio));
					clipItemsBuffer.GrowToFit(Math::Max<u64>(clipItemList.GetCount() + 32, totalCount));
				}

				clipItemsBuffer.GetBuffer(imageIndex)->UploadData(clipItemList.GetData(), clipItemList.GetCount() * sizeof(FClipItem2D));
			}

			if (shapeItemList.GetCount() > 0)
			{
				if (shapeItemsBuffer.GetElementCount() < shapeItemList.GetCount())
				{
					u64 totalCount = (u64)((f32)shapeItemsBuffer.GetElementCount() * (1.0f + shapeItemGrowRatio));
					shapeItemsBuffer.GrowToFit(Math::Max<u64>(shapeItemList.GetCount() + 64, totalCount));
				}

				shapeItemsBuffer.GetBuffer(imageIndex)->UploadData(shapeItemList.GetData(), shapeItemList.GetCount() * sizeof(FShapeItem2D));
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

	void FusionRenderer::SetItemTransform(const Matrix4x4& transform)
	{
		this->itemTransform = transform;
	}

	void FusionRenderer::SetBrush(const FBrush& brush)
	{
		currentBrush = brush;
	}

	void FusionRenderer::SetPen(const FPen& pen)
	{
		currentPen = pen;
	}

	void FusionRenderer::SetFont(const FFont& font)
	{
		if (currentFont.GetFamily() != font.GetFamily())
		{
			createNewDrawBatch = true;
		}

		currentFont = font;
	}

	void FusionRenderer::PushOpacity(f32 opacity)
	{
		if (opacityStack.IsEmpty())
		{
			opacityStack.Insert(opacity);
		}
		else
		{
			opacityStack.Insert(opacityStack.Last() * opacity);
		}
	}

	void FusionRenderer::PopOpacity()
	{
		if (!opacityStack.IsEmpty())
		{
			opacityStack.RemoveLast();
		}
	}

	void FusionRenderer::SetScreenSize(Vec2i screenSize)
	{
		this->screenSize = screenSize;
	}

	void FusionRenderer::SetDrawListTag(RHI::DrawListTag drawListTag)
	{
		this->drawListTag = drawListTag;
	}

	void FusionRenderer::PushChildCoordinateSpace(const Matrix4x4& transform)
	{
		if (coordinateSpaceStack.IsEmpty())
		{
			coordinateSpaceStack.Insert(transform);
		}
		else
		{
			coordinateSpaceStack.Insert(coordinateSpaceStack.Last() * transform);
		}
	}

	const Matrix4x4& FusionRenderer::GetTopCoordinateSpace()
	{
		if (coordinateSpaceStack.IsEmpty())
		{
			static Matrix4x4 identity = Matrix4x4::Identity();
			return identity;
		}

		return coordinateSpaceStack.Last();
	}

	void FusionRenderer::PopChildCoordinateSpace()
	{
		if (!coordinateSpaceStack.IsEmpty())
		{
			coordinateSpaceStack.RemoveLast();
		}
	}

	void FusionRenderer::PushClipShape(const Matrix4x4& clipTransform, Vec2 rectSize, const FShape& shape)
	{
		// Clipping is done via SDF functions, which require you 
		clipItemList.Insert(FClipItem2D{
				.clipTransform = (coordinateSpaceStack.Last() * clipTransform).GetInverse(),
				.cornerRadius = shape.GetCornerRadius(),
				.size = rectSize,
				.shapeType = shape.GetShapeType()
			});

		clipItemStack.Insert(clipItemList.GetCount() - 1);
	}

	void FusionRenderer::PopClipShape()
	{
		clipItemStack.RemoveLast();
	}

} // namespace CE
