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

		for (int i = 0; i < numFramesInFlight; i++)
		{
			RHI::BufferDescriptor drawItemBufferDesc{};
			drawItemBufferDesc.name = "DrawItem Buffer";
			drawItemBufferDesc.bufferSize = initialCharacterStorage * sizeof(CharacterDrawItem);
			drawItemBufferDesc.defaultHeapType = MemoryHeapType::Upload;
			drawItemBufferDesc.bindFlags = BufferBindFlags::StructuredBuffer;
			drawItemBufferDesc.structureByteStride = sizeof(CharacterDrawItem);

			charDrawItemsBuffer[i] = RHI::gDynamicRHI->CreateBuffer(drawItemBufferDesc);

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
	}

	void Renderer2D::RegisterFont(Name name, RPI::FontAtlasAsset* fontAtlasData)
	{
		if (!name.IsValid() || fontAtlasData == nullptr)
			return;

		if (defaultFont == nullptr)
		{
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
		fontStack.Push({ .fontName = defaultFont->GetName(), .fontSize = 16});

		textDrawRequests.Clear();
	}

	void Renderer2D::PushFont(Name family, u32 fontSize, bool bold)
	{
		fontStack.Push({ .fontName = family, .fontSize = fontSize, .bold = bold });
	}

	void Renderer2D::PopFont()
	{
		fontStack.Pop();
	}

	void Renderer2D::SetForeground(const Color& foreground)
	{
		this->foreground = foreground;
	}

	void Renderer2D::SetBackground(const Color& background)
	{
		this->background = background;
	}

	void Renderer2D::SetCursor(Vec2 position)
	{
		this->cursorPosition = position;
	}

	void Renderer2D::DrawText(const String& text, Vec2 size)
	{
		TextDrawRequest textRequest{};
		textRequest.font = fontStack.Top();
		textRequest.text = text;
		textRequest.position = cursorPosition;
		textRequest.size = size;
		textRequest.foreground = foreground;
		textRequest.background = background;

		textDrawRequests.Add(textRequest);
	}

	void Renderer2D::End()
	{
		fontStack.Pop(); // Default font

		const auto& vertBuffers = RPISystem::Get().GetTextQuad();
		RHI::DrawLinearArguments drawArgs = RPISystem::Get().GetTextQuadDrawArgs();

		MaterialHash currentMaterialHash = 0;
		DrawPacket* drawPacket = nullptr;

		Array<DrawPacket*> oldPackets = this->drawPackets;

		for (int i = 0; i < textDrawRequests.GetSize(); i++)
		{
			const TextDrawRequest& drawRequest = textDrawRequests[i];

			if (currentMaterialHash == 0) // First draw packet
			{
				currentMaterialHash = drawRequest.GetMaterialHash();

				if (oldPackets.NonEmpty())
				{
					drawPacket = oldPackets[0];
					oldPackets.RemoveAt(0);
				}
				else // Create new draw packet
				{
					RHI::DrawPacketBuilder builder{};
					
					
				}
			}
			else if (currentMaterialHash != drawRequest.GetMaterialHash())
			{
				// TODO: Change of material detected
			}


		}
	}

	const Array<DrawPacket*>& Renderer2D::Submit(u32 imageIndex)
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

		if (viewConstantsUpdated[imageIndex])
		{
			viewConstantsUpdated[imageIndex] = false;

			viewConstantsBuffer[imageIndex]->UploadData(&viewConstants, sizeof(viewConstants));
		}
		
		return drawPackets;
	}

	void Renderer2D::SetViewConstants(const PerViewConstants& viewConstants)
	{
		for (int i = 0; i < viewConstantsUpdated.GetSize(); i++)
		{
			viewConstantsUpdated[i] = true;
		}

		this->viewConstants = viewConstants;
	}

	void Renderer2D::ResizeCharacterDrawItemBuffer(u32 numCharactersToAdd)
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
			
			void* data;
			original->Map(0, original->GetBufferSize(), &data);
			{
				charDrawItemsBuffer[i]->UploadData(data, original->GetBufferSize());
			}
			original->Unmap();

			QueueDestroy(original);
		}
	}

	RPI::Material* Renderer2D::GetOrCreateMaterial(const TextDrawRequest& textDrawRequest)
	{
		Name fontName = textDrawRequest.font.fontName;
		bool isBold = textDrawRequest.font.bold;
		u32 fontSize = textDrawRequest.font.fontSize;
		if (fontSize < 4)
			return nullptr;

		RPI::FontAtlasAsset* fontAtlas = fontAtlasesByName[fontName];
		if (fontAtlas == nullptr)
			return nullptr;

		RPI::Material* material = materials[textDrawRequest.GetMaterialHash()];
		if (material)
			return material;

		material = new RPI::Material(textShader);
		material->SetPropertyValue("_FontAtlas", fontAtlas->GetAtlasTexture()->GetRpiTexture());
		material->SetPropertyValue("_CharacterData", fontAtlas->GetCharacterLayoutBuffer());
		material->SetPropertyValue("_BgColor", Color(0, 0, 0, 1));
		material->SetPropertyValue("_FgColor", Color(1, 1, 1, 1));
		material->SetPropertyValue("_Bold", (u32)isBold);
		material->FlushProperties();

		materials[textDrawRequest.GetMaterialHash()] = material;
		return material;
	}

} // namespace CE::RPI
