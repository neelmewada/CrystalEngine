#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API MainRenderPipeline : public CE::RenderPipeline
    {
        CE_CLASS(MainRenderPipeline, CE::RenderPipeline)
    public:

        MainRenderPipeline();
        virtual ~MainRenderPipeline();

    protected:

        void Construct() override;

        FIELD()
        MainRenderPipelineAsset* mainRenderPipelineAsset = nullptr;
        
    };
    
} // namespace CE

#include "MainRenderPipeline.rtti.h"