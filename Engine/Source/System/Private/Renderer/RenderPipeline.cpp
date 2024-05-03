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
        renderPipeline->name = GetName();
        renderPipeline->uuid = GetUuid();

        if (targetCamera != nullptr)
        {
            renderPipeline->view = targetCamera->GetRpiView();
        }

        // - Cleanup -
        
        for (int i = (int)renderPipeline->attachments.GetSize() - 1; i >= 0; --i)
        {
            renderPipeline->attachments.RemoveAt(i);
        }

        renderPipeline->passTree->Clear();

        PassImageAttachmentDesc pipelineOutputDesc{};
        pipelineOutputDesc.name = "PipelineOutput";
        pipelineOutputDesc.lifetime = AttachmentLifetimeType::External;
        pipelineOutputDesc.imageDescriptor.dimension = Dimension::Dim2D;
        pipelineOutputDesc.imageDescriptor.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;

        auto pipelineOutput = new PassAttachment(pipelineOutputDesc);
        renderPipeline->attachments.Add(pipelineOutput);
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

    RHI::DrawListTag CE::RenderPipeline::GetBuiltinDrawListTag(RPI::BuiltinDrawItemTag buitinDrawItemTag)
    {
        return RPISystem::Get().GetBuiltinDrawListTag(buitinDrawItemTag);
    }

} // namespace CE
