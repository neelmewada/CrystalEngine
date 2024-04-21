#pragma once

namespace CE
{

    CLASS()
    class SYSTEM_API RenderPipelineAsset : public Asset
    {
        CE_CLASS(RenderPipelineAsset, Asset)
    public:

        RenderPipelineAsset();

        virtual ~RenderPipelineAsset();

    protected:

    public:

        FIELD()
        MSAA msaa = MSAA2;

        FIELD()
        int directionalShadowResolution = 512;

    };
    
} // namespace CE

#include "RenderPipelineAsset.rtti.h"