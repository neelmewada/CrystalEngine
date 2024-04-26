#include "System.h"

namespace CE
{

    DirectionalShadowPass::DirectionalShadowPass()
    {
        pipelineViewTag = "DirectionalLight";
        if (!IsDefaultInstance())
        {
            drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag("shadow");
        }
    }

    DirectionalShadowPass::~DirectionalShadowPass()
    {
        if (drawListTag.IsValid())
        {
            RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
        }
    }
    
    void DirectionalShadowPass::ProduceScopes(FrameScheduler* scheduler)
    {
        String baseName = String::Format("DirectionalShadowPass_{}_", GetUuid());
        RPI::Scene* scene = renderPipeline->scene;
        
        ArrayView<ViewPtr> views = scene->GetViews(GetPipelineViewTag());

        for (View* view : views)
        {
            DrawList& drawList = view->GetDrawList(GetDrawListTag());

            
        }
    }

    void DirectionalShadowPass::EmplaceAttachments(FrameAttachmentDatabase& attachmentDatabase)
    {
        
    }

} // namespace CE
