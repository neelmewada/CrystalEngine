#include "CoreRPI.h"

namespace CE::RPI
{

	Renderer2D::Renderer2D(const Renderer2DDescriptor& desc)
		: screenSize(desc.screenSize)
		, textShader(desc.textShader)
		, drawListTag(desc.drawListTag)
		, numFramesInFlight(desc.numFramesInFlight)
		, initialCharacterStorage(desc.initialCharacterStorage)
		, characterStorageIncrement(desc.characterStorageIncrement)
		, viewConstants(desc.viewConstantData)
		, rootTransform(desc.rootTransform)
	{
		CE_ASSERT(textShader != nullptr, "Text Shader is null");
		CE_ASSERT(screenSize.x > 0 && screenSize.y > 0, "Screen size is zero!");

		numFramesInFlight = Math::Min(numFramesInFlight, RHI::Limits::MaxSwapChainImageCount);

		textQuadVertices = RPISystem::Get().GetTextQuad();
		textQuadDrawArgs = RPISystem::Get().GetTextQuadDrawArgs();

		RHI::ShaderResourceGroupLayout perViewSrgLayout = textShader->GetVariant(textShader->GetDefaultVariantIndex())->GetSrgLayout(RHI::SRGType::PerView);
		perViewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewSrgLayout);

		RHI::ShaderResourceGroupLayout perDrawSrgLayout = textShader->GetVariant(textShader->GetDefaultVariantIndex())->GetSrgLayout(RHI::SRGType::PerDraw);
		charDrawItemSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perDrawSrgLayout);

		for (int i = 0; i < numFramesInFlight; i++)
		{
			RHI::BufferDescriptor drawItemBufferDesc{};
			drawItemBufferDesc.name = "DrawItem Buffer";
			drawItemBufferDesc.bufferSize = initialCharacterStorage * sizeof(CharacterDrawItem);
			drawItemBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			drawItemBufferDesc.bindFlags = BufferBindFlags::StructuredBuffer;
			drawItemBufferDesc.structureByteStride = sizeof(CharacterDrawItem);

			charDrawItemsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(drawItemBufferDesc);
			charDrawItemSrg->Bind(i, "_DrawList", charDrawItemsBuffer[i]);

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
		charDrawItemSrg->FlushBindings();
	}

	Renderer2D::~Renderer2D()
	{
		for (int i = 0; i < numFramesInFlight; i++)
		{
			delete charDrawItemsBuffer[i];
			charDrawItemsBuffer[i] = nullptr;

			delete viewConstantsBuffer[i];
			viewConstantsBuffer[i] = nullptr;
		}

		for (const auto& [variant, material] : materials)
		{
			delete material;
		}
		materials.Clear();

		for (auto drawPacket : drawPackets)
		{
			delete drawPacket;
		}
		drawPackets.Clear();

		delete perViewSrg; perViewSrg = nullptr;
		delete charDrawItemSrg; charDrawItemSrg = nullptr;
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

		textDrawBatches.Clear();

		charDrawItemsCount = 0;
		createNewTextBatch = true;

		foreground = Color(1, 1, 1, 1);
		background = Color(0, 0, 0, 0);
	}

	void Renderer2D::PushFont(Name family, u32 fontSize, bool bold)
	{
		if (fontStack.Top().fontName != family || fontStack.Top().bold != bold)
			createNewTextBatch = true;

		fontStack.Push({ .fontName = family, .fontSize = fontSize, .bold = bold });
	}

	void Renderer2D::PopFont()
	{
		const FontInfo& last = fontStack.Top();
		const FontInfo& lastSecond = fontStack[fontStack.GetSize() - 2];

		if (last.fontName != lastSecond.fontName || last.bold != lastSecond.bold)
			createNewTextBatch = true;

		fontStack.Pop();
	}

	void Renderer2D::SetForeground(const Color& foreground)
	{
		if (this->foreground.ToVec4() != foreground.ToVec4())
		{
			createNewTextBatch = true;
		}

		this->foreground = foreground;
	}

	void Renderer2D::SetBackground(const Color& background)
	{
		if (this->background.ToVec4() != background.ToVec4())
		{
			createNewTextBatch = true;
		}

		this->background = background;
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

		if (charDrawItems.GetSize() < charDrawItemsCount + text.GetLength())
			charDrawItems.Resize(charDrawItemsCount + text.GetLength());

		const auto& metrics = fontAtlas->GetMetrics();
		f32 fontSize = font.fontSize;
		f32 atlasFontSize = metrics.fontSize;

		const float startY = cursorPosition.y + metrics.ascender * fontSize / atlasFontSize;
		const float startX = cursorPosition.x;

		float maxX = startX;
		float maxY = startY;

		Vec3 position = Vec3(startX, startY, 0);

		int totalCharactersDrawn = 0;
		int firstCharDrawItemIndex = charDrawItemsCount;

		if (textDrawBatches.IsEmpty() || createNewTextBatch)
		{
			createNewTextBatch = false;
			textDrawBatches.Add({});
			textDrawBatches.Top().firstCharDrawItemIndex = charDrawItemsCount;
			textDrawBatches.Top().foreground = foreground;
			textDrawBatches.Top().background = background;
			textDrawBatches.Top().font = font;
		}

		TextDrawBatch& curDrawBatch = textDrawBatches.Top();

		for (int i = 0; i < text.GetLength(); i++)
		{
			char c = text[i];

			if (c == '\n')
			{
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
				continue;
			}

			CharacterDrawItem& drawItem = charDrawItems[firstCharDrawItemIndex + i];
			
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

			drawItem.transform = Matrix4x4::Translation(translation) * Matrix4x4::Scale(scale);
			drawItem.charIndex = fontAtlas->GetCharacterIndex(c);
			drawItem.bgMask = 0;

			position.x += (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (position.x > maxX)
				maxX = position.x;
			if (position.y + metrics.lineHeight * fontSize / atlasFontSize > maxY)
				maxY = position.y + metrics.lineHeight * fontSize / atlasFontSize;

			totalCharactersDrawn++;
		}

		charDrawItemsCount += totalCharactersDrawn;
		curDrawBatch.charDrawItemCount += totalCharactersDrawn;

		size = Vec2(maxX - startX, maxY - startY);
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

		for (int i = 0; i < textDrawBatches.GetSize(); i++)
		{
			const TextDrawBatch& drawBatch = textDrawBatches[i];

			currentMaterialHash = drawBatch.GetMaterialHash();

			if (oldPackets.NonEmpty()) // Reuse draw packet
			{
				drawPacket = oldPackets[0];
				oldPackets.RemoveAt(0);

				RPI::Material* material = GetOrCreateMaterial(drawBatch);

				drawPacket->drawItems[0].arguments.linearArgs.firstInstance = drawBatch.firstCharDrawItemIndex;
				drawPacket->drawItems[0].arguments.linearArgs.instanceCount = drawBatch.charDrawItemCount;

				drawPacket->shaderResourceGroups[0] = material->GetShaderResourceGroup();
				drawPacket->shaderResourceGroups[1] = charDrawItemSrg;
				drawPacket->shaderResourceGroups[2] = perViewSrg;

				this->drawPackets.Add(drawPacket);
			}
			else // Create new draw packet
			{
				RHI::DrawPacketBuilder builder{};

				drawArgs.firstInstance = drawBatch.firstCharDrawItemIndex;
				drawArgs.instanceCount = drawBatch.charDrawItemCount;
				builder.SetDrawArguments(drawArgs);

				RPI::Material* material = GetOrCreateMaterial(drawBatch);

				builder.AddShaderResourceGroup(material->GetShaderResourceGroup());
				builder.AddShaderResourceGroup(charDrawItemSrg);
				builder.AddShaderResourceGroup(perViewSrg);

				// UI Item
				{
					RHI::DrawPacketBuilder::DrawItemRequest request{};
					request.vertexBufferViews.AddRange(vertBuffers);

					request.drawItemTag = drawListTag;
					request.drawFilterMask = RHI::DrawFilterMask::ALL;
					request.pipelineState = material->GetCurrentShader()->GetPipeline();

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

		if (charDrawItemsBuffer[imageIndex] != nullptr)
		{
			if (charDrawItemsBuffer[imageIndex]->GetBufferSize() < charDrawItemsCount * sizeof(CharacterDrawItem))
			{
				// TODO: Implement buffer size expansion
				//IncrementCharacterDrawItemBuffer(charDrawItemsCount + 10);
			}

			void* data;
			charDrawItemsBuffer[imageIndex]->Map(0, charDrawItemsBuffer[imageIndex]->GetBufferSize(), &data);
			{
				memcpy(data, charDrawItems.GetData(), charDrawItemsCount * sizeof(CharacterDrawItem));
			}
			charDrawItemsBuffer[imageIndex]->Unmap();
		}
		charDrawItemsCount;
		
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
		numCharactersToAdd = Math::Max(numCharactersToAdd, characterStorageIncrement);

		for (int i = 0; i < numFramesInFlight; i++)
		{
			RHI::Buffer* original = charDrawItemsBuffer[i];

			RHI::BufferDescriptor newBufferDesc{};
			newBufferDesc.name = "DrawItem Buffer";
			newBufferDesc.bindFlags = RHI::BufferBindFlags::StructuredBuffer;
			newBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			newBufferDesc.structureByteStride = sizeof(CharacterDrawItem);
			newBufferDesc.bufferSize = original->GetBufferSize() + numCharactersToAdd * sizeof(CharacterDrawItem);

			charDrawItemsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(newBufferDesc);
			charDrawItemSrg->Bind(i, "_DrawList", charDrawItemsBuffer[i]);
			
			void* data;
			original->Map(0, original->GetBufferSize(), &data);
			{
				charDrawItemsBuffer[i]->UploadData(data, original->GetBufferSize());
			}
			original->Unmap();

			QueueDestroy(original);
		}

		charDrawItemSrg->FlushBindings();
	}

	RPI::Material* Renderer2D::GetOrCreateMaterial(const TextDrawBatch& textDrawBatch)
	{
		Name fontName = textDrawBatch.font.fontName;
		bool isBold = textDrawBatch.font.bold;
		u32 fontSize = textDrawBatch.font.fontSize;
		if (fontSize < 4)
			return nullptr;

		RPI::FontAtlasAsset* fontAtlas = fontAtlasesByName[fontName];
		if (fontAtlas == nullptr)
			return nullptr;

		RPI::Material* material = materials[textDrawBatch.GetMaterialHash()];
		if (material)
			return material;

		material = new RPI::Material(textShader);
		material->SetPropertyValue("_FontAtlas", fontAtlas->GetAtlasTexture()->GetRpiTexture());
		material->SetPropertyValue("_CharacterData", fontAtlas->GetCharacterLayoutBuffer());
		material->SetPropertyValue("_BgColor", textDrawBatch.background);
		material->SetPropertyValue("_FgColor", textDrawBatch.foreground);
		material->SetPropertyValue("_Bold", (u32)isBold);
		material->FlushProperties();

		materials[textDrawBatch.GetMaterialHash()] = material;
		return material;
	}

} // namespace CE::RPI
