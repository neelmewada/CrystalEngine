#pragma once

namespace CE
{
    
    CLASS()
    class SYSTEM_API TextureCube final : public CE::Texture
    {
        CE_CLASS(TextureCube, CE::Texture)
    public:

        TextureCube();
        virtual ~TextureCube();

        RPI::Texture* GetRpiTexture() override;

        TextureDimension GetDimension() override
        {
            return TextureDimension::TexCube;
        }

        inline TextureCube* GetDiffuseConvolution() const { return diffuseConvolution; }

    private:

        FIELD()
        TextureCube* diffuseConvolution = nullptr;


#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::TextureAssetImportJob;
#endif
    };

} // namespace CE

#include "TextureCube.rtti.h"
