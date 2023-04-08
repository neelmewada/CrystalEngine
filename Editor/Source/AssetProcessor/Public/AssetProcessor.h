#pragma once

#include "Module/ModuleManager.h"

#include "AssetProcessorApplication.h"
#include "GUI/AssetImporter/AssetImporterWindow.h"

namespace CE::Editor
{
    
    class ASSETPROCESSOR_API AssetProcessorModule : public CE::Module
    {
    public:

        virtual void StartupModule() override;
        virtual void ShutdownModule() override;
        virtual void RegisterTypes() override;

    };

    class ASSETPROCESSOR_API AssetProcessor
    {
    private:
        AssetProcessor() = default;
    public:

        static AssetProcessor& Get()
        {
            static AssetProcessor instance{};
            return instance;
        }

        void ProcessAsset(IO::Path sourceAssetPath, Asset* assetInstance);
        
    };

} // namespace CE::Editor