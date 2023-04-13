
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
        if (asset == nullptr || sourceAssetPath.IsDirectory() || sourceAssetPath.GetExtension() == ".casset")
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

        PostProcessAsset(asset);
        
        SerializedObject assetSO{ asset };
        assetSO.Serialize(outPath);
    }

    void AssetProcessor::PostProcessAsset(Asset* assetInstance)
    {

    }

} // namespace CE::Editor

