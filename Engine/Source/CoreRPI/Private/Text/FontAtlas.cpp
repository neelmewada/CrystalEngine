#include "CoreRPI.h"

namespace CE::RPI
{
	FontAtlas::~FontAtlas()
	{
		delete atlas; atlas = nullptr;
	}

	FontAtlas* FontAtlas::Create(Name name, CMFontAtlas* cmFontAtlas, Object* outer)
	{
		if (!cmFontAtlas)
			return nullptr;

		const CMImage& atlasImage = cmFontAtlas->GetAtlas();
		if (!atlasImage.IsValid())
			return nullptr;

		FontAtlas* fontAtlas = CreateObject<FontAtlas>(outer, name.GetString());

		RHI::SamplerDescriptor samplerDesc{};
		samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = SamplerAddressMode::ClampToBorder;
		samplerDesc.borderColor = SamplerBorderColor::FloatOpaqueBlack;
		samplerDesc.enableAnisotropy = false;
		samplerDesc.samplerFilterMode = FilterMode::Linear;

		fontAtlas->atlas = new RPI::Texture(atlasImage, samplerDesc);

		const auto& glyphInfos = cmFontAtlas->GetGlyphInfos();
		fontAtlas->glyphLayouts.Resize(glyphInfos.GetSize());

		for (int i = 0; i < glyphInfos.GetSize(); i++)
		{
			fontAtlas->glyphLayouts[i].unicode = glyphInfos[i].charCode;
			fontAtlas->glyphLayouts[i].x0 = glyphInfos[i].x0;
			fontAtlas->glyphLayouts[i].x1 = glyphInfos[i].x1;
			fontAtlas->glyphLayouts[i].y0 = glyphInfos[i].y0;
			fontAtlas->glyphLayouts[i].y1 = glyphInfos[i].y1;

			fontAtlas->glyphLayouts[i].xOffset = glyphInfos[i].xOffset;
			fontAtlas->glyphLayouts[i].yOffset = glyphInfos[i].yOffset;

			fontAtlas->glyphLayouts[i].advance = glyphInfos[i].advance;
		}

		fontAtlas->width = atlasImage.GetWidth();
		fontAtlas->height = atlasImage.GetHeight();

		fontAtlas->CacheGlyphLayouts();

		return fontAtlas;
	}

	void FontAtlas::OnAfterDeserialize()
	{
		Super::OnAfterDeserialize();

		CacheGlyphLayouts();
	}

	void FontAtlas::CacheGlyphLayouts()
	{
		glyphLayoutCache.Clear();

		for (int i = 0; i < glyphLayouts.GetSize(); i++)
		{
			glyphLayoutCache[glyphLayouts[i].unicode] = glyphLayouts[i];
		}
	}

} // namespace CE::RPI
