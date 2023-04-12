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

    ENUM()
    enum class BuiltinAssetType
    {
        None = 0,
        SceneAsset
    };

    // Abstract: So it cannot be instantiated dynamically
    CLASS(Abstract)
    class SYSTEM_API Asset : public CE::Object
    {
        CE_CLASS(Asset, CE::Object)
    public:
        Asset(CE::Name name);
        virtual ~Asset();
        
        Name GetAssetType();

        static void RegisterAssetClass(ClassType* assetClass, String assetExtensions);
        static bool IsValidAssetType(String assetExtension);
        static ClassType* GetAssetClassFor(String assetExtension);

        static BuiltinAssetType GetBuiltinAssetTypeFor(String assetExtension);
        static String GetAssetExtensionFor(BuiltinAssetType builtinAssetType);

        inline String GetAssetName() const { return assetName; }

        inline String GetAssetExtension() const { return assetExtension; }

    protected:
        FIELD(Hidden, NonSerialized)
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
