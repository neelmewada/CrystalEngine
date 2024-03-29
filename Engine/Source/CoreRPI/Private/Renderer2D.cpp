#include "CoreRPI.h"

namespace CE::RPI
{
	constexpr bool ForceDisableBatching = false;

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

			RHI::BufferDescriptor clipRectBufferDesc{};
			clipRectBufferDesc.name = "ClipRect Buffer";
			clipRectBufferDesc.bufferSize = 100 * sizeof(Rect);
			clipRectBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			clipRectBufferDesc.bindFlags = BufferBindFlags::StructuredBuffer;
			clipRectBufferDesc.structureByteStride = sizeof(Rect);

			clipRectsBuffer[i] = gDynamicRHI->CreateBuffer(clipRectBufferDesc);
			drawItemSrg->Bind(i, "_ClipRects", clipRectsBuffer[i]);

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

			delete clipRectsBuffer[i];
			clipRectsBuffer[i] = nullptr;

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
		clipRectStack.Clear();

		drawItemCount = 0;
		clipRectCount = 0;
		createNewDrawBatch = true;

		ResetToDefaults();

		PushClipRect(Rect::FromSize(Vec2(0, 0), screenSize.ToVec2()));

		for (int i = 0; i < resubmitDrawData.GetSize(); i++)
		{
			resubmitDrawData[i] = true;
			resubmitClipRects[i] = true;
		}
	}

	void Renderer2D::ResetToDefaults()
	{
		rotation = 0.0;
		borderThickness = 0.0f;
		fillColor = Color(1, 1, 1, 1);
		outlineColor = Color(0, 0, 0, 0);
	}

	void Renderer2D::PushFont(Name family, u32 fontSize, bool bold)
	{
		if (!family.IsValid())
		{
			family = fontStack.Top().fontName;
		}

		if (fontStack.Top().fontName != family)
			createNewDrawBatch = true;

		fontStack.Push({ .fontName = family, .fontSize = fontSize, .bold = bold });
	}

	void Renderer2D::PopFont()
	{
		const FontInfo& last = fontStack.Top();
		const FontInfo& lastSecond = fontStack[fontStack.GetSize() - 2];

		if (last.fontName != lastSecond.fontName)
			createNewDrawBatch = true;

		fontStack.Pop();
	}

	void Renderer2D::PushClipRect(Rect clipRect)
	{
		if (clipRects.GetSize() < clipRectCount + 1)
		{
			clipRects.Resize(clipRectCount + 1);
		}

		if (clipRectStack.NonEmpty())
		{
			clipRect.min.y = Math::Max(clipRect.min.y, clipRects[clipRectStack.Top()].min.y);
			clipRect.max.y = Math::Min(clipRect.max.y, clipRects[clipRectStack.Top()].max.y);
			clipRect.min.x = Math::Max(clipRect.min.x, clipRects[clipRectStack.Top()].min.x);
			clipRect.max.x = Math::Min(clipRect.max.x, clipRects[clipRectStack.Top()].max.x);
		}

		clipRects[clipRectCount] = clipRect;
		clipRectStack.Push(clipRectCount);
		clipRectCount++;
	}

	void Renderer2D::PopClipRect()
	{
		clipRectStack.Pop();
	}

	bool Renderer2D::ClipRectExists()
	{
		return clipRectStack.NonEmpty();
	}

	Rect Renderer2D::GetLastClipRect()
	{
		if (!ClipRectExists())
			return Rect();
		return clipRects[clipRectStack.Top()];
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

	void Renderer2D::SetRotation(f32 degrees)
	{
		this->rotation = degrees;
	}

	void Renderer2D::SetCursor(Vec2 position)
	{
		this->cursorPosition = position;
	}

	Vec2 Renderer2D::CalculateTextSize(const String& text, f32 width)
	{
		Vec2 size{};
		size.width = width;

		const bool isFixedWidth = width > 0;

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
		int whitespaceIdx = -1;

		struct CharacterPosition
		{
			Vec3 translation;
			Vec3 scale;
		};

		static Array<CharacterPosition> positions{};
		if (positions.GetSize() < text.GetLength())
		{
			positions.Resize(text.GetLength());
		}

		for (int i = 0; i < text.GetLength(); i++)
		{
			char c = text[i];

			if (c == ' ')
			{
				whitespaceIdx = i;
			}

			if (c == '\n')
			{
				whitespaceIdx = -1;
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
				continue;
			}

			const RPI::FontGlyphLayout& glyphLayout = fontAtlas->GetGlyphLayout(c);

			const float glyphWidth = (f32)glyphLayout.GetWidth();
			const float glyphHeight = (f32)glyphLayout.GetHeight();

			position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (isFixedWidth && position.x + glyphWidth * fontSize / atlasFontSize > width)
			{
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;

				// Go through previous characters and bring them to this new-line
				if (whitespaceIdx >= 0)
				{
					position.x -= (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

					for (int j = whitespaceIdx + 1; j < i; j++)
					{
						char prevChar = text[j];
						const RPI::FontGlyphLayout& prevGlyphLayout = fontAtlas->GetGlyphLayout(prevChar);

						position.x += (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;

						position.x += (f32)prevGlyphLayout.advance * fontSize / atlasFontSize - (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;

					}
					whitespaceIdx = -1;

					position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;
				}
			}

			positions[i] = { .translation = position, .scale = Vec3((f32)glyphLayout.advance * fontSize / atlasFontSize, 0, 0) };

			position.x += (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (position.x > maxX)
				maxX = position.x;
			if (position.y + metrics.lineHeight * fontSize / atlasFontSize > maxY)
				maxY = position.y + metrics.lineHeight * fontSize / atlasFontSize;
		}

		size = Vec2(maxX - startX, maxY - startY);
		if (isFixedWidth)
			size.width = width;
		return size;
	}

	Vec2 Renderer2D::CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width)
	{
		Vec2 size{};
		size.width = width;

		const bool isFixedWidth = width > 0;
		if (!fontName.IsValid())
			fontName = defaultFontName;
		
		RPI::FontAtlasAsset* fontAtlas = fontAtlasesByName[fontName];
		if (fontAtlas == nullptr)
			return Vec2();

		const auto& metrics = fontAtlas->GetMetrics();
		f32 atlasFontSize = metrics.fontSize;

		const float startY = cursorPosition.y + metrics.ascender * fontSize / atlasFontSize;
		const float startX = cursorPosition.x;

		float maxX = startX;
		float maxY = startY;

		Vec3 position = Vec3(startX, startY, 0);
		int whitespaceIdx = -1;

		struct CharacterPosition
		{
			Vec3 translation;
			Vec3 scale;
		};

		static Array<CharacterPosition> positions{};
		if (positions.GetSize() < text.GetLength())
		{
			positions.Resize(text.GetLength());
		}

		for (int i = 0; i < text.GetLength(); i++)
		{
			char c = text[i];

			if (c == ' ')
			{
				whitespaceIdx = i;
			}

			if (c == '\n')
			{
				whitespaceIdx = -1;
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
				continue;
			}

			const RPI::FontGlyphLayout& glyphLayout = fontAtlas->GetGlyphLayout(c);

			const float glyphWidth = (f32)glyphLayout.GetWidth();
			const float glyphHeight = (f32)glyphLayout.GetHeight();

			position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (isFixedWidth && position.x + glyphWidth * fontSize / atlasFontSize > startX + width)
			{
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;

				// Go through previous characters and bring them to this new-line
				if (whitespaceIdx >= 0)
				{
					position.x -= (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

					for (int j = whitespaceIdx + 1; j < i; j++)
					{
						char prevChar = text[j];
						const RPI::FontGlyphLayout& prevGlyphLayout = fontAtlas->GetGlyphLayout(prevChar);

						position.x += (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;

						position.x += (f32)prevGlyphLayout.advance * fontSize / atlasFontSize - (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;

					}
					whitespaceIdx = -1;

					position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;
				}
			}

			positions[i] = { .translation = position, .scale = Vec3((f32)glyphLayout.advance * fontSize / atlasFontSize, 0, 0) };

			position.x += (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (position.x > maxX)
				maxX = position.x;
			if (position.y + metrics.lineHeight * fontSize / atlasFontSize > maxY)
				maxY = position.y + metrics.lineHeight * fontSize / atlasFontSize;
		}

		size = Vec2(maxX - startX, maxY - startY);
		if (isFixedWidth)
			size.width = width;
		return size;
	}

	Vec2 Renderer2D::CalculateTextOffsets(Array<Rect>& outRects, const String& text, f32 fontSize, Name fontName, f32 width)
	{
		Vec2 size{};
		size.width = width;

		const bool isFixedWidth = width > 0;
		if (!fontName.IsValid())
			fontName = defaultFontName;

		RPI::FontAtlasAsset* fontAtlas = fontAtlasesByName[fontName];
		if (fontAtlas == nullptr)
			return Vec2();

		const auto& metrics = fontAtlas->GetMetrics();
		f32 atlasFontSize = metrics.fontSize;

		const float startY = cursorPosition.y;// + metrics.ascender * fontSize / atlasFontSize;
		const float startX = cursorPosition.x;

		float maxX = startX;
		float maxY = startY;

		Vec3 position = Vec3(startX, startY, 0);
		int whitespaceIdx = -1;
		outRects.Resize(text.GetLength());

		for (int i = 0; i < text.GetLength(); i++)
		{
			char c = text[i];

			if (c == ' ')
			{
				whitespaceIdx = i;
			}

			if (c == '\n')
			{
				whitespaceIdx = -1;
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
				continue;
			}

			const RPI::FontGlyphLayout& glyphLayout = fontAtlas->GetGlyphLayout(c);

			const float glyphWidth = (f32)glyphLayout.GetWidth();
			const float glyphHeight = (f32)glyphLayout.GetHeight();

			position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			outRects[i].min = position - cursorPosition;
			outRects[i].max = outRects[i].min + Vec2(glyphWidth * fontSize / atlasFontSize, glyphHeight * fontSize / atlasFontSize);
			//outRects[i].max.x = outRects[i].min.x + (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;
			//outRects[i].max.y = outRects[i].min.y + glyphHeight * fontSize / atlasFontSize;

			if (isFixedWidth && position.x + glyphWidth * fontSize / atlasFontSize > startX + width)
			{
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;

				// Go through previous characters and bring them to this new-line
				if (whitespaceIdx >= 0)
				{
					position.x -= (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

					for (int j = whitespaceIdx + 1; j < i; j++)
					{
						char prevChar = text[j];
						const RPI::FontGlyphLayout& prevGlyphLayout = fontAtlas->GetGlyphLayout(prevChar);

						position.x += (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;

						outRects[j].min = position - cursorPosition;
						outRects[j].max = outRects[j].min + Vec2(glyphWidth * fontSize / atlasFontSize, glyphHeight * fontSize / atlasFontSize);

						position.x += (f32)prevGlyphLayout.advance * fontSize / atlasFontSize - (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;

					}
					whitespaceIdx = -1;

					position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;
				}
			}

			position.x += (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (position.x > maxX)
				maxX = position.x;
			if (position.y + metrics.lineHeight * fontSize / atlasFontSize > maxY)
				maxY = position.y + metrics.lineHeight * fontSize / atlasFontSize;
		}

		size = Vec2(maxX - startX, maxY - startY);
		if (isFixedWidth)
			size.width = width;
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

		Matrix4x4 rotationMat = Quat::EulerDegrees(Vec3(0, 0, rotation)).ToMatrix();

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
			drawBatches.Add({});
			drawBatches.Top().firstDrawItemIndex = drawItemCount;
			drawBatches.Top().font = font;
		}

		DrawBatch& curDrawBatch = drawBatches.Top();
		int whitespaceIdx = -1;
		int idx = 0;

		for (int i = 0; i < text.GetLength(); i++)
		{
			char c = text[i];

			if (c == ' ')
			{
				whitespaceIdx = i;
			}

			if (c == '\n')
			{
				whitespaceIdx = -1;
				position.x = startX;
				position.y += metrics.lineHeight * fontSize / atlasFontSize;
				continue;
			}

			DrawItem2D& drawItem = drawItems[firstDrawItemIndex + totalCharactersDrawn];
			
			const RPI::FontGlyphLayout& glyphLayout = fontAtlas->GetGlyphLayout(c);

			Vec3 scale = Vec3(1, 1, 1);
			const float glyphWidth = (f32)glyphLayout.GetWidth();
			const float glyphHeight = (f32)glyphLayout.GetHeight();

			// Need to multiply by 2 because final range is [-w, w] instead of [0, w] (which is double the size)
			scale.x = glyphWidth * fontSize / atlasFontSize * 2;
			scale.y = glyphHeight * fontSize / atlasFontSize * 2;

			position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

			if (isFixedWidth && position.x + glyphWidth * fontSize / atlasFontSize > startX + size.width)
			{
				// Go through previous characters and bring them to this new-line
				if (whitespaceIdx >= 0)
				{
					position.x = startX;
					position.y += metrics.lineHeight * fontSize / atlasFontSize;

					position.x -= (f32)glyphLayout.xOffset * fontSize / atlasFontSize;

					for (int j = whitespaceIdx + 1; j < i; j++)
					{
						char prevChar = text[j];

						const RPI::FontGlyphLayout& prevGlyphLayout = fontAtlas->GetGlyphLayout(prevChar);

						DrawItem2D& prevDrawItem = drawItems[firstDrawItemIndex + j];

						position.x += (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;

						Vec2 prevQuadPos = position;
						prevQuadPos.y -= (f32)prevGlyphLayout.yOffset * fontSize / atlasFontSize;

						// Need to multiply by 2 because final range is [-w, w] instead of [0, w] (which is double the size)
						Vec3 prevTranslation = Vec3(prevQuadPos.x * 2, prevQuadPos.y * 2, 0);
						
						prevDrawItem.transform = rotationMat * Matrix4x4::Translation(prevTranslation) * Matrix4x4::Scale(prevDrawItem.itemSize);

						position.x += (f32)prevGlyphLayout.advance * fontSize / atlasFontSize - (f32)prevGlyphLayout.xOffset * fontSize / atlasFontSize;
						
					}
					whitespaceIdx = -1;

					position.x += (f32)glyphLayout.xOffset * fontSize / atlasFontSize;
				}
			}
			
			Vec2 quadPos = position;
			quadPos.y -= (f32)glyphLayout.yOffset * fontSize / atlasFontSize;

			// Need to multiply by 2 because final range is [-w, w] instead of [0, w] (which is double the size)
			Vec3 translation = Vec3(quadPos.x * 2, quadPos.y * 2, 0);
			
			drawItem.fillColor = outlineColor.ToVec4();
			drawItem.outlineColor = outlineColor.ToVec4();
			drawItem.itemSize = scale;
			drawItem.borderThickness = 0.0f;
			drawItem.bold = font.bold ? 1 : 0;
			drawItem.drawType = DRAW_Text;
			drawItem.clipRectIdx = clipRectStack.Top();
			
			drawItem.transform = rotationMat * Matrix4x4::Translation(translation) * Matrix4x4::Scale(scale);
			drawItem.charIndex = fontAtlas->GetCharacterIndex(c);

			position.x += (f32)glyphLayout.advance * fontSize / atlasFontSize - (f32)glyphLayout.xOffset * fontSize / atlasFontSize;
			
			if (position.x > maxX)
				maxX = position.x;
			if (position.y + metrics.lineHeight * fontSize / atlasFontSize > maxY)
				maxY = position.y + metrics.lineHeight * fontSize / atlasFontSize;

			totalCharactersDrawn++;
			idx++;
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

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
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
		drawItem.clipRectIdx = clipRectStack.Top();

		curDrawBatch.drawItemCount++;
		
		drawItemCount++;
		return size;
	}

	Vec2 Renderer2D::DrawRect(Vec2 size)
	{
		if (size.x <= 0 || size.y <= 0)
			return Vec2(0, 0);

		const FontInfo& font = fontStack.Top();

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
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
		drawItem.clipRectIdx = clipRectStack.Top();

		curDrawBatch.drawItemCount++;

		drawItemCount++;
		return size;
	}

	Vec2 Renderer2D::DrawRoundedRect(Vec2 size, const Vec4& cornerRadius)
	{
		if (size.x <= 0 || size.y <= 0)
			return Vec2(0, 0);

		const FontInfo& font = fontStack.Top();

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
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

		drawItem.transform = Matrix4x4::Translation(translation) * Quat::EulerDegrees(Vec3(0, 0, rotation)).ToMatrix() * Matrix4x4::Scale(scale);
		drawItem.drawType = DRAW_RoundedRect;
		drawItem.fillColor = fillColor.ToVec4();
		drawItem.outlineColor = outlineColor.ToVec4();
		drawItem.cornerRadius = cornerRadius;
		drawItem.itemSize = size;
		drawItem.borderThickness = borderThickness;
		drawItem.bold = 0;
		drawItem.clipRectIdx = clipRectStack.Top();

		curDrawBatch.drawItemCount++;

		drawItemCount++;
		return size;
	}

	Vec2 Renderer2D::DrawRoundedX(Vec2 size)
	{
		if (size.x <= 0 || size.y <= 0)
			return Vec2(0, 0);

		const FontInfo& font = fontStack.Top();

		if constexpr (ForceDisableBatching)
		{
			createNewDrawBatch = true;
		}

		if (drawBatches.IsEmpty() || createNewDrawBatch)
		{
			createNewDrawBatch = false;
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

		drawItem.transform = Matrix4x4::Translation(translation) * Quat::EulerDegrees(Vec3(0, 0, rotation)).ToMatrix() * Matrix4x4::Scale(scale);
		drawItem.drawType = DRAW_RoundedX;
		drawItem.fillColor = outlineColor.ToVec4();
		drawItem.outlineColor = outlineColor.ToVec4();
		drawItem.itemSize = size;
		drawItem.borderThickness = borderThickness;
		drawItem.bold = 0;
		drawItem.clipRectIdx = clipRectStack.Top();
		
		curDrawBatch.drawItemCount++;

		drawItemCount++;
		return size;
	}

	void Renderer2D::End()
	{
		PopClipRect();

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

		while (oldPackets.NonEmpty())
		{
			delete oldPackets[0];
			oldPackets.RemoveAt(0);
		}
	}

	const Array<DrawPacket*>& Renderer2D::FlushDrawPackets(u32 imageIndex)
	{
		curImageIndex = (int)imageIndex;

		// - Destroy queued resources only when they are out of usage scope

		for (int i = (int)destructionQueue.GetSize() - 1; i >= 0; i--)
		{
			destructionQueue[i].frameCounter++;

			if (destructionQueue[i].frameCounter > (int)RHI::Limits::MaxSwapChainImageCount * 2)
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

		// - Update Item Buffer -

		if (drawItemsBuffer[imageIndex] != nullptr && resubmitDrawData[imageIndex])
		{
			resubmitDrawData[imageIndex] = false;

			if (drawItemsBuffer[imageIndex]->GetBufferSize() < drawItemCount * sizeof(DrawItem2D))
			{
				IncrementCharacterDrawItemBuffer(drawItemCount + 10);
			}
			
			void* data;
			drawItemsBuffer[imageIndex]->Map(0, drawItemsBuffer[imageIndex]->GetBufferSize(), &data);
			{
				memcpy(data, drawItems.GetData(), drawItemCount * sizeof(DrawItem2D));
			}
			drawItemsBuffer[imageIndex]->Unmap();
		}

		// - Update Clip Rects Buffer -

		if (clipRectsBuffer[imageIndex] != nullptr && resubmitClipRects[imageIndex])
		{
			resubmitClipRects[imageIndex] = false;

			if (clipRectsBuffer[imageIndex]->GetBufferSize() < clipRectCount * sizeof(Rect))
			{
				IncrementClipRectsBuffer(clipRectCount + 10);
			}

			void* data;
			clipRectsBuffer[imageIndex]->Map(0, clipRectsBuffer[imageIndex]->GetBufferSize(), &data);
			{
				memcpy(data, clipRects.GetData(), clipRectCount * sizeof(Rect));
			}
			clipRectsBuffer[imageIndex]->Unmap();
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
		u32 curNumItems = drawItemsBuffer[0]->GetBufferSize() / sizeof(DrawItem2D);
		u32 incrementCount = curNumItems * 3 / 2; // Add 50% to the storage

		numCharactersToAdd = Math::Max(numCharactersToAdd, incrementCount);

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

	void Renderer2D::IncrementClipRectsBuffer(u32 numRectsToAdd)
	{
		u32 curNumRects = clipRectsBuffer[0]->GetBufferSize() / sizeof(Rect);
		u32 incrementCount = curNumRects * 3 / 2;

		numRectsToAdd = Math::Max(numRectsToAdd, incrementCount);

		for (int i = 0; i < numFramesInFlight; ++i)
		{
			RHI::Buffer* original = clipRectsBuffer[i];

			RHI::BufferDescriptor newBufferDesc{};
			newBufferDesc.name = "ClipRects Buffer";
			newBufferDesc.bindFlags = RHI::BufferBindFlags::StructuredBuffer;
			newBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			newBufferDesc.structureByteStride = sizeof(Rect);
			newBufferDesc.bufferSize = original->GetBufferSize() + numRectsToAdd * sizeof(Rect);

			clipRectsBuffer[i] = gDynamicRHI->CreateBuffer(newBufferDesc);
			drawItemSrg->Bind(i, "_ClipRect", clipRectsBuffer[i]);

			void* data;
			original->Map(0, original->GetBufferSize(), &data);
			{
				clipRectsBuffer[i]->UploadData(data, original->GetBufferSize());
			}
			original->Unmap();

			QueueDestroy(original);
		}
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
