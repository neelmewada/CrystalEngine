#include "System.h"

namespace CE::Editor
{
	const Array<String>& TextureAssetDefinition::GetSourceExtensions()
	{
		static Array<String> extensions = {
			".png", ".jpg"
		};
		return extensions;
	}

	ClassType* TextureAssetDefinition::GetAssetClass()
	{
		return GetStaticClass<Texture2D>();
	}

	ClassType* TextureAssetDefinition::GetAssetClass(const String& extension)
	{
		return GetAssetClass();
	}

} // namespace CE
