#pragma once

namespace CE
{
	CLASS()
	class FUSIONCORE_API FFontManager : public Object
	{
		CE_CLASS(FFontManager, Object)
	public:

		FFontManager();

		void Init();
		void Shutdown();

		bool RegisterFont(const Name& fontName, const Array<CharRange>& characterSets, MemoryStream* ttfFontFile);

	private:

		HashMap<Name, FFontAtlas*> fontAtlases;

		FT_Library ft = nullptr;

	};

} // namespace CE

#include "FFontManager.rtti.h"