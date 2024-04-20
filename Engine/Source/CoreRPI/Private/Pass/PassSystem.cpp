#include "CoreRPI.h"

namespace CE::RPI
{

    void PassSystem::Initialize()
    {
		// - Pass Templates -

        // - Depth Stencil Pass -
        {
            RasterPass* depthPass = CreateObject<RasterPass>(GetTransientPackage(MODULE_NAME), "DepthStencilPass");

            PassSlot depthStencilOutput{};
            depthStencilOutput.name = "DepthStencilOutput";
            depthStencilOutput.slotType = PassSlotType::Output;
            depthStencilOutput.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
            depthStencilOutput.dimensions = { Dimension::Dim2D };
            depthStencilOutput.formats = { Format::D32_SFLOAT_S8_UINT, Format::D24_UNORM_S8_UINT, Format::D16_UNORM_S8_UINT };
            depthStencilOutput.loadStoreAction.loadAction = AttachmentLoadAction::Clear;
            depthStencilOutput.loadStoreAction.storeAction = AttachmentStoreAction::Store;
            depthStencilOutput.loadStoreAction.loadActionStencil = AttachmentLoadAction::Clear;
            depthStencilOutput.loadStoreAction.storeActionStencil = AttachmentStoreAction::Store;
            depthStencilOutput.loadStoreAction.clearValueDepth = 1.0f;
            depthStencilOutput.loadStoreAction.clearValueStencil = 0;

            depthPass->AddSlot(depthStencilOutput);

            RegisterTemplate(depthPass);
        }

        // - Depth Pass -
        {
            RasterPass* depthPass = CreateObject<RasterPass>(GetTransientPackage(MODULE_NAME), "DepthPass");

            PassSlot depthOutput{};
            depthOutput.name = "DepthOutput";
            depthOutput.slotType = PassSlotType::Output;
            depthOutput.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
            depthOutput.dimensions = { Dimension::Dim2D };
            depthOutput.formats = { Format::D32_SFLOAT, Format::D32_SFLOAT_S8_UINT, Format::D24_UNORM_S8_UINT, Format::D16_UNORM_S8_UINT };
            depthOutput.loadStoreAction.loadAction = AttachmentLoadAction::Clear;
            depthOutput.loadStoreAction.storeAction = AttachmentStoreAction::Store;
            depthOutput.loadStoreAction.loadActionStencil = AttachmentLoadAction::DontCare;
            depthOutput.loadStoreAction.storeActionStencil = AttachmentStoreAction::DontCare;
            depthOutput.loadStoreAction.clearValueDepth = 1.0f;

            depthPass->AddSlot(depthOutput);

            RegisterTemplate(depthPass);
        }

        // - Skybox Pass -
        {
            RasterPass* skyboxPass = CreateObject<RasterPass>(GetTransientPackage(MODULE_NAME), "SkyboxPass");

            PassSlot colorOutput{};
            colorOutput.name = "ColorOutput";
            colorOutput.slotType = PassSlotType::Output;
            colorOutput.attachmentUsage = ScopeAttachmentUsage::Color;
            colorOutput.dimensions = { Dimension::Dim2D };
            colorOutput.formats = {};
            colorOutput.loadStoreAction.loadAction = AttachmentLoadAction::Clear;
            colorOutput.loadStoreAction.storeAction = AttachmentStoreAction::Store;
            colorOutput.loadStoreAction.clearValue = Vec4(0, 0, 0, 0);

            skyboxPass->AddSlot(colorOutput);

            RegisterTemplate(skyboxPass);
        }

        // - Opaque Pass -
        {
            RasterPass* opaquePass = CreateObject<RasterPass>(GetTransientPackage(MODULE_NAME), "OpaquePass");

            PassSlot depthInput{};
            depthInput.name = "DepthInput";
            depthInput.slotType = PassSlotType::Input;
            depthInput.attachmentUsage = ScopeAttachmentUsage::DepthStencil;
            depthInput.dimensions = { Dimension::Dim2D };
            depthInput.formats = { Format::D32_SFLOAT, Format::D32_SFLOAT_S8_UINT, Format::D24_UNORM_S8_UINT, Format::D16_UNORM_S8_UINT };
            depthInput.loadStoreAction.loadAction = AttachmentLoadAction::Load;
            depthInput.loadStoreAction.storeAction = AttachmentStoreAction::Store;
            depthInput.loadStoreAction.loadActionStencil = AttachmentLoadAction::DontCare;
            depthInput.loadStoreAction.storeActionStencil = AttachmentStoreAction::DontCare;
            
            PassSlot colorOutput{};
            colorOutput.name = "ColorOutput";
            colorOutput.slotType = PassSlotType::InputOutput;
            colorOutput.attachmentUsage = ScopeAttachmentUsage::Color;
            colorOutput.dimensions = { Dimension::Dim2D };
            colorOutput.formats = { };
            colorOutput.loadStoreAction.loadAction = AttachmentLoadAction::Load;
            colorOutput.loadStoreAction.storeAction = AttachmentStoreAction::Store;

            opaquePass->AddSlot(depthInput);
            opaquePass->AddSlot(colorOutput);

            RegisterTemplate(opaquePass);
        }
    }

    void PassSystem::Shutdown()
    {
        for (auto [name, pass] : passTemplates)
        {
            if (pass)
				pass->Destroy();
        }
        passTemplates.Clear();
    }

    PassSystem& PassSystem::Get()
    {
        static PassSystem instance{};
        return instance;
    }

    void PassSystem::RegisterTemplate(Name templateName, Pass* pass)
    {
        if (passTemplates[templateName] != nullptr)
            return;

        passTemplates[templateName] = pass;
    }

    void PassSystem::RegisterTemplate(Pass* pass)
    {
        if (pass)
        {
            RegisterTemplate(pass->GetName(), pass);
        }
    }

    Pass* PassSystem::CreatePass(Name templateName, Name newPassName)
    {
        if (passTemplates[templateName] == nullptr)
            return nullptr;

        if (!newPassName.IsValid())
            newPassName = templateName;

        Pass* templatePass = passTemplates[templateName];

        Pass* newPass = CreateObject<Pass>(GetTransientPackage(MODULE_NAME), newPassName.GetString(), OF_NoFlags, templatePass->GetClass());
        newPass->slots = templatePass->slots;

        return newPass;
    }

} // namespace CE::RPI
