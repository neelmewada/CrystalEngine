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

    protected:

        void OnAfterConstruct() override final;

        virtual void Construct();

        RPI::RenderPipeline* renderPipeline = nullptr;

        FIELD()
        RenderPipelineAsset* renderPipelineAsset = nullptr;

    };
    
} // namespace CE

#include "RenderPipeline.rtti.h"