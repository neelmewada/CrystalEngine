#pragma once

#include "CoreMinimal.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class AssetProcessor;
    }
#endif

    CLASS(Abstract)
    class SYSTEM_API Asset : public CE::Object
    {
        CE_CLASS(Asset, CE::Object)
    protected:
        Asset(CE::Name name);
    public:
        virtual ~Asset();
        
        Name GetAssetType();

        static void RegisterAssetClass(ClassType* assetClass, String assetExtensions);
        static bool IsValidAssetType(String assetExtension);
        static ClassType* GetAssetClassFor(String assetExtension);

    protected:
        FIELD(Hidden)
        String assetName{};

        FIELD(Hidden)
        String assetExtension{};
        
#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::AssetProcessor;
#endif

    private:
        static HashMap<Name, ClassType*> extensionToAssetClassMap;
    };

} // namespace CE

#include "Asset.rtti.h"

#define CE_REGISTER_ASSET_TYPE(AssetClass, ...)\
CE_REGISTER_TYPES(AssetClass)\
CE::Asset::RegisterAssetClass(AssetClass::Type(), "" #__VA_ARGS__)
