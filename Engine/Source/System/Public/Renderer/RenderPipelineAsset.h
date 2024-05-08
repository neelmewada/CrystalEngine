#pragma once

namespace CE
{
    class RenderPipeline;
    
    CLASS(Abstract)
    class SYSTEM_API RenderPipelineAsset : public Asset
    {
        CE_CLASS(RenderPipelineAsset, Asset)
    public:

        RenderPipelineAsset();

        virtual ~RenderPipelineAsset();
        
        void ApplyChanges();
        
    protected:

        FIELD()
        Array<CE::RenderPipeline*> renderPipelines = {};

    public:

        FIELD()
        MSAA msaa = MSAA2;

        FIELD()
        int directionalShadowResolution = 512;

        friend class CE::RenderPipeline;
    };
    
} // namespace CE

#include "RenderPipelineAsset.rtti.h"