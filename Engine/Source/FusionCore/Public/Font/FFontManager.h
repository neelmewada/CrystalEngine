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

		const Name& GetDefaultFontFamily() const;

		bool RegisterFont(const Name& fontName, const Array<CharRange>& characterSets, MemoryStream* ttfFontFile);

		bool DeregisterFont(const Name& fontName);

		FFontAtlas* FindFont(const Name& fontName);

		//! @brief Flushes all the changes to GPU
		void Flush(u32 imageIndex);

	private:

		HashMap<Name, FFontAtlas*> fontAtlases;

		FT_Library ft = nullptr;

	};

} // namespace CE

#include "FFontManager.rtti.h"