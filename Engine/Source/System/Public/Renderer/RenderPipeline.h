#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API RenderPipeline : public Object
    {
        CE_CLASS(RenderPipeline, Object)
    public:

        RenderPipeline();

        virtual ~RenderPipeline();

        RPI::RenderPipeline* GetRpiRenderPipeline() const { return renderPipeline; }

        virtual void ConstructPipeline();
        
    protected:

        RPI::RenderPipeline* renderPipeline = nullptr;

        FIELD()
        RenderPipelineAsset* renderPipelineAsset = nullptr;

    };
    
} // namespace CE

#include "RenderPipeline.rtti.h"