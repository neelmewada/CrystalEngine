#include "System.h"

namespace CE
{

    CE::RenderPipeline::RenderPipeline()
    {
        if (!IsDefaultInstance())
        {
	        renderPipeline = new RPI::RenderPipeline();
        }
    }

    CE::RenderPipeline::~RenderPipeline()
    {
        delete renderPipeline; renderPipeline = nullptr;
    }

    void CE::RenderPipeline::ConstructPipeline()
    {
        // - Cleanup -
        
        for (int i = (int)renderPipeline->attachments.GetSize() - 1; i >= 0; --i)
        {
            if (renderPipeline->attachments[i]->name != "PipelineOutput")
            {
                renderPipeline->attachments.RemoveAt(i);
            }
        }

        renderPipeline->passTree->Clear();
    }
    
    void CE::RenderPipeline::SetRenderPipelineAsset(RenderPipelineAsset* renderPipelineAsset)
    {
        if (this->renderPipelineAsset == renderPipelineAsset)
            return;

        if (this->renderPipelineAsset != nullptr)
        {
            this->renderPipelineAsset->renderPipelines.Remove(this);
        }
        
        this->renderPipelineAsset = renderPipelineAsset;

        if (this->renderPipelineAsset != nullptr)
        {
            this->renderPipelineAsset->renderPipelines.Add(this);
        }

        MarkDirty();
    }
    
    void CE::RenderPipeline::Tick()
    {
        if (isDirty)
        {
            isDirty = false;

            ConstructPipeline();
        }
    }

} // namespace CE
