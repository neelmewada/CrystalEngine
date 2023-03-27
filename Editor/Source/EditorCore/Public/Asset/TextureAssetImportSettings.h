#pragma once

#include "CoreMinimal.h"

#include "AssetImportSettings.h"

#include "Asset/TextureAsset.h"

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API TextureAssetImportSettings : public AssetImportSettings
    {
        CE_CLASS(TextureAssetImportSettings, CE::Editor::AssetImportSettings)
    public:

        virtual bool ApplyImportSettings(Asset* targetAsset) override;

    protected:

        FIELD()
        CE::TextureDataType textureType{};
    };

} // namespace CE::Editor

#include "TextureAssetImportSettings.rtti.h"
