#include "CoreRPI.h"

namespace CE::RPI
{
	
	FontAtlasAsset::~FontAtlasAsset()
	{
		delete characterLayoutBuffer; characterLayoutBuffer = nullptr;
	}

	RHI::Buffer* FontAtlasAsset::GetCharacterLayoutBuffer()
	{
		if (glyphLayoutCache.IsEmpty())
		{
			CacheGlyphLayouts();
		}

		if (!characterLayoutBuffer && glyphLayouts.NotEmpty())
		{
			RHI::BufferDescriptor bufferDesc{};
			bufferDesc.name = "Character Buffer";
			bufferDesc.bindFlags = BufferBindFlags::StructuredBuffer;
			bufferDesc.bufferSize = glyphLayouts.GetSize() * sizeof(Vec4);
			bufferDesc.defaultHeapType = MemoryHeapType::Default;
			bufferDesc.structureByteStride = sizeof(Vec4);

			characterLayoutBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);
			
			Vec4* data = new Vec4[glyphLayouts.GetSize()];
			defer(
				delete[] data;
			);

			for (int i = 0; i < glyphLayouts.GetSize(); i++)
			{
				const auto& glyphLayout = glyphLayouts[i];
				
				Vec4& atlasUV = *(data + i);
				atlasUV.left = (f32)glyphLayout.x0 / fontAtlasTexture->width;
				atlasUV.top = (f32)glyphLayout.y0 / fontAtlasTexture->height;
				atlasUV.right = (f32)glyphLayout.x1 / fontAtlasTexture->width;
				atlasUV.bottom = (f32)glyphLayout.y1 / fontAtlasTexture->height;
			}

			characterLayoutBuffer->UploadData(data, characterLayoutBuffer->GetBufferSize());
		}

		return characterLayoutBuffer;
	}

	void FontAtlasAsset::OnAfterDeserialize()
	{
		Super::OnAfterDeserialize();

		CacheGlyphLayouts();
	}

	void FontAtlasAsset::CacheGlyphLayouts()
	{
		glyphLayoutCache.Clear();
		characterIndexByUnicode.Clear();

		bool nullFound = false;

		for (int i = 0; i < glyphLayouts.GetSize(); i++)
		{
			if (glyphLayouts[i].unicode == 0)
			{
				nullFound = true;
			}

			glyphLayoutCache[glyphLayouts[i].unicode] = glyphLayouts[i];
			characterIndexByUnicode[glyphLayouts[i].unicode] = i;
		}

		if (!nullFound)
		{
			RPI::FontGlyphLayout glyph{};
			glyph.advance = 0;
			glyph.x0 = glyph.x1 = glyph.y0 = glyph.y1 = glyph.xOffset = glyph.yOffset = 0;
			glyph.unicode = 0;

			glyphLayouts.Add(glyph);

			characterIndexByUnicode[0] = glyphLayouts.GetSize() - 1;
		}
	}

} // namespace CE::RPI
