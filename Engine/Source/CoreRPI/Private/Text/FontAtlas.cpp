#include "CoreRPI.h"

namespace CE::RPI
{
	FontAtlas::~FontAtlas()
	{
		delete atlas; atlas = nullptr;
	}

	FontAtlas* FontAtlas::Create(Name name, RPI::Texture* atlas, const FontAtlasLayout& atlasLayout, Object* outer)
	{
		FontAtlas* instance = CreateObject<FontAtlas>(outer, name.GetString());
		instance->atlas = atlas;
		instance->atlasLayout = atlasLayout;

		return instance;
	}

} // namespace CE::RPI
