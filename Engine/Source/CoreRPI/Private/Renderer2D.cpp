#include "CoreRPI.h"

namespace CE::RPI
{

	Renderer2D::Renderer2D(const Renderer2DDescriptor& desc)
		: screenSize(desc.screenSize)
		//, textShader(desc.textShader)
		, drawShader(desc.drawShader)
		, multisampling(desc.multisampling)
		, drawListTag(desc.drawListTag)
		, numFramesInFlight(desc.numFramesInFlight)
		, initialDrawItemStorage(desc.initialDrawItemStorage)
		, drawItemStorageIncrement(desc.drawItemStorageIncrement)
		, viewConstants(desc.viewConstantData)
		, rootTransform(desc.rootTransform)
	{
		CE_ASSERT(drawShader != nullptr, "Draw Shader is null");
		CE_ASSERT(screenSize.x > 0 && screenSize.y > 0, "Screen size is zero!");

		numFramesInFlight = Math::Min(numFramesInFlight, RHI::Limits::MaxSwapChainImageCount);

		textQuadVertices = RPISystem::Get().GetTextQuad();
		textQuadDrawArgs = RPISystem::Get().GetTextQuadDrawArgs();

		defaultMaterial = new RPI::Material(drawShader);
		defaultMaterial->SelectVariant(drawShader->GetDefaultVariantIndex());

		RHI::ShaderResourceGroupLayout perViewSrgLayout = defaultMaterial->GetCurrentShader()->GetSrgLayout(RHI::SRGType::PerView);
		perViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);

		RHI::ShaderResourceGroupLayout perDrawSrgLayout = defaultMaterial->GetCurrentShader()->GetSrgLayout(RHI::SRGType::PerDraw);
		drawItemSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perDrawSrgLayout);

		for (int i = 0; i < numFramesInFlight; i++)
		{
			RHI::BufferDescriptor drawItemBufferDesc{};
			drawItemBufferDesc.name = "DrawItem Buffer";
			drawItemBufferDesc.bufferSize = initialDrawItemStorage * sizeof(DrawItem2D);
			drawItemBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			drawItemBufferDesc.bindFlags = BufferBindFlags::StructuredBuffer;
			drawItemBufferDesc.structureByteStride = sizeof(DrawItem2D);

			drawItemsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(drawItemBufferDesc);
			drawItemSrg->Bind(i, "_DrawList", drawItemsBuffer[i]);

			RHI::BufferDescriptor viewConstantsBufferDesc{};
			viewConstantsBufferDesc.name = "ViewConstants";
			viewConstantsBufferDesc.bufferSize = sizeof(viewConstants);
			viewConstantsBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			viewConstantsBufferDesc.bindFlags = BufferBindFlags::ConstantBuffer;
			viewConstantsBufferDesc.structureByteStride = sizeof(viewConstants);

			viewConstantsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(viewConstantsBufferDesc);
			viewConstantsBuffer[i]->UploadData(&viewConstants, sizeof(viewConstants));
			perViewSrg->Bind(i, "_PerViewData", viewConstantsBuffer[i]);
		}
		
		perViewSrg->FlushBindings();
		drawItemSrg->FlushBindings();
	}

	Renderer2D::~Renderer2D()
	{
		for (int i = 0; i < numFramesInFlight; i++)
		{
			delete drawItemsBuffer[i];
			drawItemsBuffer[i] = nullptr;

			delete viewConstantsBuffer[i];
			viewConstantsBuffer[i] = nullptr;
		}

		for (const auto& [variant, material] : materials)
		{
			delete material;
		}
		materials.Clear();

		delete defaultMaterial;

		for (auto drawPacket : drawPackets)
		{
			delete drawPacket;
		}
		drawPackets.Clear();

		delete perViewSrg; perViewSrg = nullptr;
		delete drawItemSrg; drawItemSrg = nullptr;
	}

	void Renderer2D::RegisterFont(Name name, RPI::FontAtlasAsset* fontAtlasData)
	{
		if (!name.IsValid() || fontAtlasData == nullptr)
			return;

		if (defaultFont == nullptr)
		{
			defaultFontName = name;
			defaultFont = fontAtlasData;
		}

		fontAtlasesByName[name] = fontAtlasData;
	}

	void Renderer2D::SetScreenSize(Vec2i newScreenSize)
	{
		this->screenSize = newScreenSize;
	}

	void Renderer2D::Begin()
	{
		fontStack.Push({ .fontName = defaultFontName, .fontSize = 16});

		drawBatches.Clear();

		drawItemCount = 0;
		createNewTextBatch = true;

		fillColor = Color(1, 1, 1, 1);
		outlineColor = Color(0, 0, 0, 0);
	}

	void Renderer2D::PushFont(Name family, u32 fontSize, bool bold)
	{
		if (fontStack.Top().fontName != family)
			createNewTextBatch = true;

		fontStack.Push({ .fontName = family, .fontSize = fontSize, .bold = bold });
	}

	void Renderer2D::PopFont()
	{
		const FontInfo& last = fontStack.Top();
		const FontInfo& lastSecond = fontStack[fontStack.GetSize() - 2];

		if (last.fontName != lastSecond.fontName)
			createNewTextBatch = true;

		fontStack.Pop();
	}

	void Renderer2D::SetFillColor(const Color& color)
	{
		this->fillColor = color;
	}

	void Renderer2D::SetOutlineColor(const Color& color)
	{
		this->outlineColor = color;
	}

	void Renderer2D::SetBorderThickness(f32 thickness)
	{
		this->borderThickness = thickness;
	}

	void Renderer2D::SetCursor(Vec2 position)
	{
		this->cursorPosition = position;
	}

	Vec2 Renderer2D::CalculateTextSize(const String& text)
	{
		Vec2 size{};

		const FontInfo& font = fontStack.Top();

		RPI::FontAtlasAsset* fontAtlas = fontAtlasesByName[font.fontName];
		if (fontAtlas == nullptr)
			return Vec2();

		const auto& metrics = fontAtlas->GetMetrics();
		f32 fontSize = font.fontSize;
		f32 atlasFontSize = metrics.fontSize;

		const float startY = cursorPosition.y + metrics.ascender * fontSize / atlasFontSize;
		const float startX = cursorPosition.x;

		float maxX = startX;
		float maxY = startY;

		Vec3 position = Vec3(startX, startY, 0);

		for (int i = 0; i < text.GetLength(); i++)
		{
			char c = text[i];

			if (c == '\n')
			{
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
				continue;
			}

			const RPI::FontGlyphLayout& glyphLayout = fontAtlas->GetGlyphLayout(c);

			position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			position.x += (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (position.x > maxX)
				maxX = position.x;
			if (position.y + metrics.lineHeight * fontSize / atlasFontSize > maxY)
				maxY = position.y + metrics.lineHeight * fontSize / atlasFontSize;
		}

		size = Vec2(maxX - startX, maxY - startY);
		return size;
	}

	Vec2 Renderer2D::DrawText(const String& text, Vec2 size)
	{
		if (text.IsEmpty())
			return Vec2();

		const bool isFixedWidth = size.width > 0;
		const bool isFixedHeight = size.height > 0;

		const FontInfo& font = fontStack.Top();

		RPI::FontAtlasAsset* fontAtlas = fontAtlasesByName[font.fontName];
		if (fontAtlas == nullptr)
			return Vec2();

		if (drawItems.GetSize() < drawItemCount + text.GetLength())
			drawItems.Resize(drawItemCount + text.GetLength());

		const auto& metrics = fontAtlas->GetMetrics();
		f32 fontSize = font.fontSize;
		f32 atlasFontSize = metrics.fontSize;
		
		const float startY = cursorPosition.y + metrics.ascender * fontSize / atlasFontSize;
		const float startX = cursorPosition.x;

		float maxX = startX;
		float maxY = startY;

		Vec3 position = Vec3(startX, startY, 0);

		int totalCharactersDrawn = 0;
		int firstDrawItemIndex = drawItemCount;

		if (drawBatches.IsEmpty() || createNewTextBatch)
		{
			createNewTextBatch = false;
			drawBatches.Add({});
			drawBatches.Top().firstDrawItemIndex = drawItemCount;
			drawBatches.Top().font = font;
		}

		DrawBatch& curDrawBatch = drawBatches.Top();

		for (int i = 0; i < text.GetLength(); i++)
		{
			char c = text[i];

			if (c == '\n')
			{
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
				continue;
			}

			DrawItem2D& drawItem = drawItems[firstDrawItemIndex + i];
			
			const RPI::FontGlyphLayout& glyphLayout = fontAtlas->GetGlyphLayout(c);

			Vec3 scale = Vec3(1, 1, 1);
			const float glyphWidth = (f32)glyphLayout.GetWidth();
			const float glyphHeight = (f32)glyphLayout.GetHeight();

			// Need to multiply by 2 because final range is [-w, w] instead of [0, w]
			scale.x = glyphWidth * fontSize / atlasFontSize * 2;
			scale.y = glyphHeight * fontSize / atlasFontSize * 2;

			position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (isFixedWidth && position.x > size.width)
			{
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
			}
			
			Vec2 quadPos = position;
			quadPos.y -= (f32)glyphLayout.yOffset * fontSize / atlasFontSize;

			Vec3 translation = Vec3(quadPos.x * 2, quadPos.y * 2, 0);
			
			drawItem.fillColor = fillColor.ToVec4();
			drawItem.outlineColor = outlineColor.ToVec4();
			drawItem.itemSize = scale;
			drawItem.borderThickness = 0.0f;
			drawItem.bold = 0;
			drawItem.drawType = DRAW_Text;
			
			drawItem.transform = Matrix4x4::Translation(translation) * Matrix4x4::Scale(scale);
			drawItem.charIndex = fontAtlas->GetCharacterIndex(c);

			position.x += (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;
			
			if (position.x > maxX)
				maxX = position.x;
			if (position.y + metrics.lineHeight * fontSize / atlasFontSize > maxY)
				maxY = position.y + metrics.lineHeight * fontSize / atlasFontSize;

			totalCharactersDrawn++;
		}

		drawItemCount += totalCharactersDrawn;
		curDrawBatch.drawItemCount += totalCharactersDrawn;

		size = Vec2(maxX - startX, maxY - startY);
		return size;
	}

	Vec2 Renderer2D::DrawCircle(Vec2 size)
	{
		if (size.x <= 0 || size.y <= 0)
			return Vec2(0, 0);

		const FontInfo& font = fontStack.Top();

		if (drawBatches.IsEmpty() || createNewTextBatch)
		{
			createNewTextBatch = false;
			drawBatches.Add({});
			drawBatches.Top().firstDrawItemIndex = drawItemCount;
			drawBatches.Top().font = font;
		}

		if (drawItems.GetSize() < drawItemCount + 1)
			drawItems.Resize(drawItemCount + 1);

		DrawBatch& curDrawBatch = drawBatches.Top();

		DrawItem2D& drawItem = drawItems[drawItemCount];
		
		Vec3 scale = Vec3(1, 1, 1);

		// Need to multiply by 2 because final range is [-w, w] instead of [0, w]
		scale.x = size.width * 2;
		scale.y = size.height * 2;

		Vec2 quadPos = cursorPosition;
		Vec3 translation = Vec3(quadPos.x * 2, quadPos.y * 2, 0);

		drawItem.transform = Matrix4x4::Translation(translation) * Matrix4x4::Scale(scale);
		drawItem.drawType = DRAW_Circle;
		drawItem.fillColor = fillColor.ToVec4();
		drawItem.outlineColor = outlineColor.ToVec4();
		drawItem.itemSize = size;
		drawItem.borderThickness = borderThickness;
		drawItem.bold = 0;

		curDrawBatch.drawItemCount++;
		
		drawItemCount++;
		return size;
	}

	Vec2 Renderer2D::DrawRect(Vec2 size)
	{
		if (size.x <= 0 || size.y <= 0)
			return Vec2(0, 0);

		const FontInfo& font = fontStack.Top();

		if (drawBatches.IsEmpty() || createNewTextBatch)
		{
			createNewTextBatch = false;
			drawBatches.Add({});
			drawBatches.Top().firstDrawItemIndex = drawItemCount;
			drawBatches.Top().font = font;
		}

		if (drawItems.GetSize() < drawItemCount + 1)
			drawItems.Resize(drawItemCount + 1);

		DrawBatch& curDrawBatch = drawBatches.Top();

		DrawItem2D& drawItem = drawItems[drawItemCount];

		Vec3 scale = Vec3(1, 1, 1);

		// Need to multiply by 2 because final range is [-w, w] instead of [0, w]
		scale.x = size.width * 2;
		scale.y = size.height * 2;

		Vec2 quadPos = cursorPosition;
		Vec3 translation = Vec3(quadPos.x * 2, quadPos.y * 2, 0);

		drawItem.transform = Matrix4x4::Translation(translation) * Matrix4x4::Scale(scale);
		drawItem.drawType = DRAW_Rect;
		drawItem.fillColor = fillColor.ToVec4();
		drawItem.outlineColor = outlineColor.ToVec4();
		drawItem.itemSize = size;
		drawItem.borderThickness = borderThickness;
		drawItem.bold = 0;

		curDrawBatch.drawItemCount++;

		drawItemCount++;
		return size;
	}

	void Renderer2D::End()
	{
		fontStack.Pop(); // Default font

		const auto& vertBuffers = RPISystem::Get().GetTextQuad();
		RHI::DrawLinearArguments drawArgs = RPISystem::Get().GetTextQuadDrawArgs();

		MaterialHash currentMaterialHash = 0;
		DrawPacket* drawPacket = nullptr;

		Array<DrawPacket*> oldPackets = this->drawPackets;
		this->drawPackets.Clear();

		for (int i = 0; i < drawBatches.GetSize(); i++)
		{
			const DrawBatch& drawBatch = drawBatches[i];

			if (oldPackets.NonEmpty()) // Reuse draw packet
			{
				drawPacket = oldPackets[0];
				oldPackets.RemoveAt(0);

				RPI::Material* material = GetOrCreateMaterial(drawBatch);

				drawPacket->drawItems[0].arguments.linearArgs.firstInstance = drawBatch.firstDrawItemIndex;
				drawPacket->drawItems[0].arguments.linearArgs.instanceCount = drawBatch.drawItemCount;

				drawPacket->shaderResourceGroups[0] = material->GetShaderResourceGroup();
				drawPacket->shaderResourceGroups[1] = drawItemSrg;
				drawPacket->shaderResourceGroups[2] = perViewSrg;

				this->drawPackets.Add(drawPacket);
			}
			else // Create new draw packet
			{
				RHI::DrawPacketBuilder builder{};

				drawArgs.firstInstance = drawBatch.firstDrawItemIndex;
				drawArgs.instanceCount = drawBatch.drawItemCount;
				builder.SetDrawArguments(drawArgs);

				RPI::Material* material = GetOrCreateMaterial(drawBatch);

				builder.AddShaderResourceGroup(material->GetShaderResourceGroup());
				builder.AddShaderResourceGroup(drawItemSrg);
				builder.AddShaderResourceGroup(perViewSrg);

				// UI Item
				{
					RHI::DrawPacketBuilder::DrawItemRequest request{};
					request.vertexBufferViews.AddRange(vertBuffers);

					request.drawItemTag = drawListTag;
					request.drawFilterMask = RHI::DrawFilterMask::ALL;
					request.pipelineState = material->GetCurrentShader()->GetPipeline(multisampling);

					builder.AddDrawItem(request);
				}

				drawPacket = builder.Build();
				this->drawPackets.Add(drawPacket);
			}
		}

	}

	const Array<DrawPacket*>& Renderer2D::FlushDrawPackets(u32 imageIndex)
	{
		curImageIndex = imageIndex;

		// - Destroy queued resources

		for (int i = destructionQueue.GetSize() - 1; i >= 0; i--)
		{
			if (destructionQueue[i].imageIndex == imageIndex)
			{
				delete destructionQueue[i].buffer;
				delete destructionQueue[i].srg;
				delete destructionQueue[i].material;
				destructionQueue.RemoveAt(i);
			}
		}

		// - Update View Constants

		if (viewConstantsUpdateRequired[imageIndex])
		{
			viewConstantsUpdateRequired[imageIndex] = false;

			viewConstantsBuffer[imageIndex]->UploadData(&viewConstants, sizeof(viewConstants));
		}

		// - Flush Material Properties -

		for (auto [hash, material] : materials)
		{
			material->FlushProperties(imageIndex);
		}

		// - Update Draw Lists -

		if (drawItemsBuffer[imageIndex] != nullptr)
		{
			if (drawItemsBuffer[imageIndex]->GetBufferSize() < drawItemCount * sizeof(DrawItem2D))
			{
				// TODO: Implement buffer size expansion
				//IncrementCharacterDrawItemBuffer(drawItemCount + 10);
			}
			
			void* data;
			drawItemsBuffer[imageIndex]->Map(0, drawItemsBuffer[imageIndex]->GetBufferSize(), &data);
			{
				memcpy(data, drawItems.GetData(), drawItemCount * sizeof(DrawItem2D));
			}
			drawItemsBuffer[imageIndex]->Unmap();
		}
		
		return drawPackets;
	}

	void Renderer2D::SetViewConstants(const PerViewConstants& viewConstants)
	{
		for (int i = 0; i < viewConstantsUpdateRequired.GetSize(); i++)
		{
			viewConstantsUpdateRequired[i] = true;
		}

		this->viewConstants = viewConstants;
	}

	void Renderer2D::IncrementCharacterDrawItemBuffer(u32 numCharactersToAdd)
	{
		numCharactersToAdd = Math::Max(numCharactersToAdd, drawItemStorageIncrement);

		for (int i = 0; i < numFramesInFlight; i++)
		{
			RHI::Buffer* original = drawItemsBuffer[i];

			RHI::BufferDescriptor newBufferDesc{};
			newBufferDesc.name = "DrawItem Buffer";
			newBufferDesc.bindFlags = RHI::BufferBindFlags::StructuredBuffer;
			newBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			newBufferDesc.structureByteStride = sizeof(DrawItem2D);
			newBufferDesc.bufferSize = original->GetBufferSize() + numCharactersToAdd * sizeof(DrawItem2D);

			drawItemsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(newBufferDesc);
			drawItemSrg->Bind(i, "_DrawList", drawItemsBuffer[i]);
			
			void* data;
			original->Map(0, original->GetBufferSize(), &data);
			{
				drawItemsBuffer[i]->UploadData(data, original->GetBufferSize());
			}
			original->Unmap();

			QueueDestroy(original);
		}

		drawItemSrg->FlushBindings();
	}

	RPI::Material* Renderer2D::GetOrCreateMaterial(const DrawBatch& drawBatch)
	{
		Name fontName = drawBatch.font.fontName;
		bool isBold = drawBatch.font.bold;
		u32 fontSize = drawBatch.font.fontSize;
		if (fontSize < 4)
			return nullptr;

		RPI::FontAtlasAsset* fontAtlas = fontAtlasesByName[fontName];
		if (fontAtlas == nullptr)
			return nullptr;

		RPI::Material* material = materials[drawBatch.GetMaterialHash()];
		if (material)
			return material;

		material = new RPI::Material(drawShader);
		material->SetPropertyValue("_FontAtlas", fontAtlas->GetAtlasTexture()->GetRpiTexture());
		material->SetPropertyValue("_CharacterData", fontAtlas->GetCharacterLayoutBuffer());
		material->SetPropertyValue("_SDFSmoothness", 50.0f);
		material->FlushProperties();

		materials[drawBatch.GetMaterialHash()] = material;
		return material;
	}

} // namespace CE::RPI
