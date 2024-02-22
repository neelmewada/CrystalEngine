#pragma once

namespace CE
{

    struct SYSTEM_API SamplerState
    {
        CE_STRUCT(SamplerState)
    public:

        FIELD()
        SamplerAddressMode addressModeU{};

        FIELD()
        SamplerAddressMode addressModeV{};

        FIELD()
        SamplerAddressMode addressModeW{};

        FIELD()
        FilterMode samplerFilterMode{};

        FIELD()
        SamplerBorderColor borderColor{};

        FIELD()
        b8 enableAnisotropy = false;

        FIELD()
        u8 maxAnisotropy = 16;

        inline SIZE_T GetHash() const
        {
            SIZE_T hash = CE::GetHash(addressModeU);
            CombineHash(hash, addressModeV);
            CombineHash(hash, addressModeW);
            CombineHash(hash, samplerFilterMode);
            CombineHash(hash, borderColor);
            CombineHash(hash, enableAnisotropy);
            if (enableAnisotropy)
            {
                CombineHash(hash, maxAnisotropy);
            }
            return hash;
        }

        inline RHI::SamplerDescriptor GetRHIDescriptor() const
        {
            RHI::SamplerDescriptor desc{};
            desc.addressModeU = addressModeU;
            desc.addressModeV = addressModeV;
            desc.addressModeW = addressModeW;
            desc.samplerFilterMode = samplerFilterMode;
            desc.borderColor = borderColor;
            desc.enableAnisotropy = enableAnisotropy;
            desc.maxAnisotropy = maxAnisotropy;
            return desc;
        }
    };
    
} // namespace CE

#include "SamplerState.rtti.h"
