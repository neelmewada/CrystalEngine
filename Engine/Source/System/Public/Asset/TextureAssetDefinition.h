#pragma once

namespace CE
{
    
	CLASS()
	class SYSTEM_API TextureAssetDefinition : public AssetDefinition
	{
		CE_CLASS(TextureAssetDefinition, AssetDefinition)
	public:

		const Array<String>& GetSourceExtensions() override;

		ClassType* GetAssetClass() override;

		ClassType* GetAssetClass(const String& extension) override;

	protected:


	};

} // namespace CE

#include "TextureAssetDefinition.rtti.h"
