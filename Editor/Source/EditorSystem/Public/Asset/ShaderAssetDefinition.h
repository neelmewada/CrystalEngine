#pragma once

namespace CE::Editor
{
    
	CLASS()
	class EDITORSYSTEM_API ShaderAssetDefinition : public AssetDefinition
	{
		CE_CLASS(ShaderAssetDefinition, AssetDefinition)
	public:

		ShaderAssetDefinition();
		virtual ~ShaderAssetDefinition();

		const Array<String>& GetSourceExtensions() override;

		ClassType* GetAssetClass() override;

		ClassType* GetAssetClass(const String& extension) override;

		SubClassType<AssetImporter> GetAssetImporterClass() override;

	protected:


	};

} // namespace CE::Editor

#include "ShaderAssetDefinition.rtti.h"
