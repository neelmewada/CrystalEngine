
#include "AssetProcessor.h"

CE_IMPLEMENT_MODULE(AssetProcessor, CE::Editor::AssetProcessorModule)

namespace CE::Editor
{
	void AssetProcessorModule::StartupModule()
	{

	}

	void AssetProcessorModule::ShutdownModule()
	{

	}

	void AssetProcessorModule::RegisterTypes()
	{

	}

    void AssetProcessor::ProcessAsset(IO::Path sourceAssetPath, AssetImportSettings* importSettings)
    {
        if (importSettings == nullptr || sourceAssetPath.IsDirectory())
            return;
        ClassType* assetClass = AssetImportSettings::GetAssetClassFor((ClassType*)importSettings->GetType());
        if (assetClass == nullptr || !assetClass->CanBeInstantiated())
            return;
        
        Asset* asset = (Asset*)assetClass->CreateDefaultInstance();
        importSettings->ApplyImportSettings(asset);
        
        asset->assetName = sourceAssetPath.GetFilename().RemoveExtension().GetString();
        auto extension = sourceAssetPath.GetFilename().GetExtension().GetString();
        if (!extension.IsEmpty())
            asset->assetExtension = extension.GetSubstring(1);
        
        IO::Path outPath = sourceAssetPath.GetParentPath() / (asset->assetName + ".casset");
        
        if (outPath.Exists())
        {
            IO::Path::Remove(outPath);
        }
        
        SerializedObject assetSO{ asset };
        assetSO.Serialize(outPath);
        
        assetClass->DestroyInstance(asset);
    }

} // namespace CE::Editor

