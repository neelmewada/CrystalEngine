#include "CoreRPI.h"

namespace CE::RPI
{

    TextureAsset::~TextureAsset()
    {
        delete rpiTexture;
        rpiTexture = nullptr;
    }

    RPI::Texture* TextureAsset::GetRpiTexture()
    {
        if (!rpiTexture)
        {
			RPI::TextureDescriptor desc{};
			desc.samplerDesc.addressModeU = addressModeU;
			desc.samplerDesc.addressModeV = addressModeV;
			desc.samplerDesc.addressModeW = desc.samplerDesc.addressModeU;

			desc.texture.width = width;
			desc.texture.height = height;
			desc.texture.depth = 1;
			desc.texture.arrayLayers = arrayLayers;
			desc.texture.name = GetName();
			desc.texture.bindFlags = RHI::TextureBindFlags::ShaderRead;
			desc.texture.mipLevels = mipLevels;
			desc.texture.sampleCount = 1;
			desc.texture.dimension = dimension;
			desc.texture.format = format;

			desc.source = &source;

			rpiTexture = new RPI::Texture(desc);
        }

        return rpiTexture;
    }

} // namespace CE::RPI
