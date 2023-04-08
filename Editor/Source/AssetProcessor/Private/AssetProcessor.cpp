
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

    void AssetProcessor::ProcessAsset(IO::Path sourceAssetPath, Asset* asset)
    {
        if (sourceAssetPath.IsDirectory())
            return;
        
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
    }

} // namespace CE::Editor

