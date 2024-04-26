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

    void MainRenderPipeline::ConstructPipeline()
    {
	    Super::ConstructPipeline();

        PassTree* passTree = renderPipeline->passTree;
    	renderPipeline->mainViewTag = "MainCamera";

        ParentPass* rootPass = passTree->GetRootPass();

        // -------------------------------
        // Attachments
        // -------------------------------

        PassAttachment* pipelineOutput = renderPipeline->FindAttachment("PipelineOutput");

		// - Depth Stencil
    	
        PassAttachment* depthStencilAttachment;
	    {
            PassImageAttachmentDesc depthStencilAttachmentDesc{};
            depthStencilAttachmentDesc.name = "DepthStencil";
            depthStencilAttachmentDesc.lifetime = AttachmentLifetimeType::Transient;
            depthStencilAttachmentDesc.sizeSource.source = pipelineOutput->name;

            depthStencilAttachmentDesc.imageDescriptor.format = Format::D32_SFLOAT_S8_UINT;
            depthStencilAttachmentDesc.imageDescriptor.arrayLayers = 1;
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

    	// - Color MSAA
    	
        PassAttachment* colorMsaa;
	    {
            PassImageAttachmentDesc colorMsaaAttachmentDesc{};
            colorMsaaAttachmentDesc.name = "ColorMSAA";
            colorMsaaAttachmentDesc.lifetime = AttachmentLifetimeType::Transient;
            colorMsaaAttachmentDesc.sizeSource.source = pipelineOutput->name;

            colorMsaaAttachmentDesc.imageDescriptor.format = Format::R8G8B8A8_UNORM;
            colorMsaaAttachmentDesc.imageDescriptor.mipCount = 1;
            colorMsaaAttachmentDesc.imageDescriptor.arrayLayers = 1;
            colorMsaaAttachmentDesc.imageDescriptor.dimension = Dimension::Dim2D;
            colorMsaaAttachmentDesc.imageDescriptor.bindFlags = TextureBindFlags::Color;
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

    	// - Directional Shadow Map

        PassAttachment* directionalShadowMapList; // Directional shadow maps are always externally managed
	    {
            PassImageAttachmentDesc directionalShadowMapListDesc{};
            directionalShadowMapListDesc.name = "DirectionalShadowMapList";
            directionalShadowMapListDesc.lifetime = AttachmentLifetimeType::External;
            directionalShadowMapListDesc.sizeSource.fixedSizes = Vec3i(1, 1, 1) * mainRenderPipelineAsset->directionalShadowResolution;

            directionalShadowMapListDesc.imageDescriptor.format = Format::D32_SFLOAT;
            directionalShadowMapListDesc.imageDescriptor.mipCount = 1;
            directionalShadowMapListDesc.imageDescriptor.arrayLayers = 1;
            directionalShadowMapListDesc.imageDescriptor.dimension = Dimension::Dim2D;
            directionalShadowMapListDesc.imageDescriptor.sampleCount = 1;
            directionalShadowMapListDesc.imageDescriptor.bindFlags = TextureBindFlags::DepthStencil | TextureBindFlags::ShaderRead;
            directionalShadowMapListDesc.fallbackFormats = { Format::D32_SFLOAT_S8_UINT, Format::D24_UNORM_S8_UINT, Format::D16_UNORM_S8_UINT };

            directionalShadowMapList = renderPipeline->AddAttachment(directionalShadowMapListDesc);
	    }

        // -------------------------------
        // Passes
        // -------------------------------

        // - Depth Pass

        RasterPass* depthPass = (RasterPass*)PassSystem::Get().CreatePass("DepthPass");
	    {
            PassAttachmentBinding outputBinding{};
            outputBinding.slotType = PassSlotType::Output;
            outputBinding.attachment = depthStencilAttachment;
            outputBinding.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
            outputBinding.name = "DepthOutput";
            outputBinding.connectedBinding = outputBinding.fallbackBinding = nullptr;

            depthPass->AddAttachmentBinding(outputBinding);
            
            rootPass->AddChild(depthPass);
	    }

        // - Skybox Pass

        RasterPass* skyboxPass = (RasterPass*)PassSystem::Get().CreatePass("SkyboxPass");
	    {
            PassAttachmentBinding colorOutputBinding{};
            colorOutputBinding.slotType = PassSlotType::Output;
            colorOutputBinding.attachment = colorMsaa;
            colorOutputBinding.attachmentUsage = ScopeAttachmentUsage::Color;
            colorOutputBinding.name = "ColorOutput";
            colorOutputBinding.connectedBinding = colorOutputBinding.fallbackBinding = nullptr;

            skyboxPass->AddAttachmentBinding(colorOutputBinding);

            rootPass->AddChild(skyboxPass);
	    }

        // - Directional Shadow Pass

        RasterPass* shadowPass = CreateObject<RasterPass>(GetTransientPackage(MODULE_NAME), "DirectionalShadowPass");
	    {
            // Array of Texture2D<float> i.e. Shadow maps
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

                PassAttachmentBinding shadowMapListBinding{};
                shadowMapListBinding.name = "DirectionalShadowListOutput";
                shadowMapListBinding.slotType = PassSlotType::Output;
                shadowMapListBinding.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
                shadowMapListBinding.attachment = directionalShadowMapList;
                shadowMapListBinding.connectedBinding = shadowMapListBinding.fallbackBinding = nullptr;

                shadowPass->AddAttachmentBinding(shadowMapListBinding);
            }

            rootPass->AddChild(shadowPass);
	    }

        // - Opaque Pass

        auto opaquePass = CreateObject<RasterPass>(GetTransientPackage(MODULE_NAME), "OpaquePass");
	    {
            // DepthInput
            {
                PassSlot depthSlot{};
                depthSlot.name = "DepthInput";
                depthSlot.slotType = PassSlotType::Input;
                depthSlot.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
                depthSlot.loadStoreAction.loadAction = AttachmentLoadAction::Load;
                depthSlot.loadStoreAction.storeAction = AttachmentStoreAction::Store;

                opaquePass->AddSlot(depthSlot);

                PassAttachmentBinding depthSlotBinding{};
                depthSlotBinding.name = "DepthInput";
                depthSlotBinding.slotType = PassSlotType::Input;
                depthSlotBinding.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
                depthSlotBinding.connectedBinding = depthPass->FindOutputBinding("DepthOutput");

                opaquePass->AddAttachmentBinding(depthSlotBinding);
            }

            // ColorMSAA
            {
                PassSlot colorSlot{};
                colorSlot.name = "ColorMSAA";
                colorSlot.slotType = PassSlotType::InputOutput;
                colorSlot.attachmentUsage = ScopeAttachmentUsage::Color;
                colorSlot.loadStoreAction.loadAction = AttachmentLoadAction::Load;
                colorSlot.loadStoreAction.storeAction = AttachmentStoreAction::Store;

                opaquePass->AddSlot(colorSlot);

                PassAttachmentBinding colorBinding{};
                colorBinding.name = "ColorMSAA";
                colorBinding.slotType = PassSlotType::InputOutput;
                colorBinding.attachmentUsage = ScopeAttachmentUsage::Color;
                colorBinding.connectedBinding = skyboxPass->FindOutputBinding("ColorOutput");

                opaquePass->AddAttachmentBinding(colorBinding);
            }

            rootPass->AddChild(opaquePass);
	    }

        // - Resolve Pass

        auto resolvePass = (RasterPass*)PassSystem::Get().CreatePass("ResolvePass");
	    {
		    {
                PassAttachmentBinding colorBinding{};
                colorBinding.name = "ColorMSAA";
                colorBinding.slotType = PassSlotType::InputOutput;
                colorBinding.attachmentUsage = ScopeAttachmentUsage::Color;
                colorBinding.connectedBinding = opaquePass->FindInputOutputBinding("ColorMSAA");
                colorBinding.fallbackBinding = nullptr;

                resolvePass->AddAttachmentBinding(colorBinding);
		    }
	    	
		    {
                PassAttachmentBinding resolveBinding{};
                resolveBinding.name = "Resolve";
                resolveBinding.slotType = PassSlotType::Output;
                resolveBinding.attachmentUsage = ScopeAttachmentUsage::Resolve;
                resolveBinding.attachment = pipelineOutput;
                resolveBinding.connectedBinding = resolveBinding.fallbackBinding = nullptr;

                resolvePass->AddAttachmentBinding(resolveBinding);
		    }

            rootPass->AddChild(resolvePass);
	    }
    }

} // namespace CE
