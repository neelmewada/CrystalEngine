#include "CoreRPI.h"

namespace CE::RPI
{
	
	FontAtlasAsset::~FontAtlasAsset()
	{

	}

	void FontAtlasAsset::OnAfterDeserialize()
	{
		Super::OnAfterDeserialize();

		CacheGlyphLayouts();
	}

	void FontAtlasAsset::CacheGlyphLayouts()
	{
		glyphLayoutCache.Clear();

		for (int i = 0; i < glyphLayouts.GetSize(); i++)
		{
			glyphLayoutCache[glyphLayouts[i].unicode] = glyphLayouts[i];
		}
	}

} // namespace CE::RPI
