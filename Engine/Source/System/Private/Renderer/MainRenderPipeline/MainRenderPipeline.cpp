#include "System.h"

namespace CE
{

    MainRenderPipeline::MainRenderPipeline()
    {
        mainRenderPipelineAsset = CreateDefaultSubobject<MainRenderPipelineAsset>("MainRenderPipelineAsset");
        renderPipelineAsset = mainRenderPipelineAsset;
    }

    MainRenderPipeline::~MainRenderPipeline()
    {
	    
    }

    void MainRenderPipeline::Construct()
    {
	    Super::Construct();

        PassTree* passTree = renderPipeline->passTree;

        ParentPass* rootPass = passTree->GetRootPass();

        // -------------------------------
        // - Attachments

        PassAttachment* depthStencilAttachment;
	    {
            PassImageAttachmentDesc depthStencilAttachmentDesc{};
            depthStencilAttachmentDesc.name = "DepthStencil";
            depthStencilAttachmentDesc.lifetime = AttachmentLifetimeType::Transient;
            depthStencilAttachmentDesc.sizeSource.source = "PipelineOutput";

            depthStencilAttachmentDesc.imageDescriptor.format = Format::D32_SFLOAT_S8_UINT;
            depthStencilAttachmentDesc.imageDescriptor.arraySize = 1;
            depthStencilAttachmentDesc.imageDescriptor.mipCount = 1;
            depthStencilAttachmentDesc.imageDescriptor.dimension = Dimension::Dim2D;
            depthStencilAttachmentDesc.imageDescriptor.bindFlags = TextureBindFlags::DepthStencil;
            depthStencilAttachmentDesc.fallbackFormats = { Format::D24_UNORM_S8_UINT, Format::D16_UNORM_S8_UINT };

            switch (mainRenderPipelineAsset->msaa)
            {
            case MSAA1:
                depthStencilAttachmentDesc.imageDescriptor.sampleCount = 1;
                break;
            case MSAA2:
                depthStencilAttachmentDesc.imageDescriptor.sampleCount = 2;
                break;
            case MSAA4:
                depthStencilAttachmentDesc.imageDescriptor.sampleCount = 4;
                break;
            case MSAA8:
                depthStencilAttachmentDesc.imageDescriptor.sampleCount = 8;
                break;
            }

            depthStencilAttachment = renderPipeline->AddAttachment(depthStencilAttachmentDesc);
	    }

        PassAttachment* colorMsaa;
	    {
            PassImageAttachmentDesc colorMsaaAttachmentDesc{};
            colorMsaaAttachmentDesc.name = "ColorMSAA";
            colorMsaaAttachmentDesc.lifetime = AttachmentLifetimeType::Transient;
            colorMsaaAttachmentDesc.sizeSource.source = "PipelineOutput";

            colorMsaaAttachmentDesc.imageDescriptor.format = Format::R8G8B8A8_UNORM;
            colorMsaaAttachmentDesc.imageDescriptor.mipCount = 1;
            colorMsaaAttachmentDesc.imageDescriptor.arraySize = 1;
            colorMsaaAttachmentDesc.imageDescriptor.dimension = Dimension::Dim2D;
            colorMsaaAttachmentDesc.fallbackFormats = { Format::B8G8R8A8_UNORM };

            switch (mainRenderPipelineAsset->msaa)
            {
            case MSAA1:
                colorMsaaAttachmentDesc.imageDescriptor.sampleCount = 1;
	            break;
            case MSAA2:
                colorMsaaAttachmentDesc.imageDescriptor.sampleCount = 2;
	            break;
            case MSAA4:
                colorMsaaAttachmentDesc.imageDescriptor.sampleCount = 4;
	            break;
            case MSAA8:
                colorMsaaAttachmentDesc.imageDescriptor.sampleCount = 8;
	            break;
            }

            colorMsaa = renderPipeline->AddAttachment(colorMsaaAttachmentDesc);
	    }

        // -------------------------------
        // - Passes

	    {
            RasterPass* depthPass = (RasterPass*)PassSystem::Get().CreatePass("DepthPass");

            PassAttachmentBinding outputBinding{};
            outputBinding.slotType = PassSlotType::Output;
            outputBinding.attachment = depthStencilAttachment;
            outputBinding.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
            outputBinding.name = "DepthOutput";
            outputBinding.connectedBinding = outputBinding.fallbackBinding = nullptr;
            
            rootPass->AddChild(depthPass);
	    }

        // -------------------------------
        // - Skybox Pass

	    {
            RasterPass* skyboxPass = (RasterPass*)PassSystem::Get().CreatePass("SkyboxPass");

            PassAttachmentBinding colorOutputBinding{};
            colorOutputBinding.slotType = PassSlotType::Output;
            colorOutputBinding.attachment = colorMsaa;
            colorOutputBinding.attachmentUsage = ScopeAttachmentUsage::Color;
            colorOutputBinding.name = "ColorOutput";
            colorOutputBinding.connectedBinding = colorOutputBinding.fallbackBinding = nullptr;

            rootPass->AddChild(skyboxPass);
	    }

        // -------------------------------
        // - Directional Shadow Pass

	    {
            RasterPass* shadowPass = CreateObject<RasterPass>(GetTransientPackage(MODULE_NAME), "DirectionalShadowPass");

            {
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

                shadowPass->AddSlot(outputSlot);
            }

            
	    }
    }

} // namespace CE
