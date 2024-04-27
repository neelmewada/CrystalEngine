#include "System.h"

namespace CE
{

    DirectionalShadowPass::DirectionalShadowPass()
    {
        viewTag = "DirectionalLight";

        PassSlot outputSlot{};
        outputSlot.name = "DirectionalShadowListOutput";
        outputSlot.slotType = PassSlotType::Output;
        outputSlot.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
        outputSlot.dimensions = { Dimension::Dim2D };
        outputSlot.formats = { Format::D32_SFLOAT, Format::D32_SFLOAT_S8_UINT, Format::D24_UNORM_S8_UINT, Format::D16_UNORM_S8_UINT };
        outputSlot.loadStoreAction.loadAction = AttachmentLoadAction::Clear;
        outputSlot.loadStoreAction.storeAction = AttachmentStoreAction::Store;
        outputSlot.loadStoreAction.loadActionStencil = AttachmentLoadAction::DontCare;
        outputSlot.loadStoreAction.storeActionStencil = AttachmentStoreAction::DontCare;
        outputSlot.loadStoreAction.clearValueDepth = 1.0f;
        outputSlot.isArray = true;
        outputSlot.shaderInputName = "DirectionalShadowMapList";

        AddSlot(outputSlot);
        
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
        
        ArrayView<ViewPtr> views = scene->GetViews(GetViewTag());

        for (View* view : views)
        {
            DrawList& drawList = view->GetDrawList(GetDrawListTag());

            
        }
    }

    void DirectionalShadowPass::EmplaceAttachments(FrameAttachmentDatabase& attachmentDatabase)
    {
        
    }

} // namespace CE
