#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    static VkBlendOp RHIBlendOpToVk(RHI::BlendOp blendOp)
    {
        switch (blendOp)
        {
        case RHI::BlendOp::Add:
            return VK_BLEND_OP_ADD;
        case RHI::BlendOp::Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case RHI::BlendOp::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case RHI::BlendOp::Min:
            return VK_BLEND_OP_MIN;
        case RHI::BlendOp::Max:
            return VK_BLEND_OP_MAX;
        }
        return {};
    }

    static VkBlendFactor RHIBlendFactorToVk(RHI::BlendFactor blendFactor)
    {
        switch (blendFactor)
        {
        case RHI::BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case RHI::BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;
        case RHI::BlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case RHI::BlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

        case RHI::BlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case RHI::BlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

        case RHI::BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case RHI::BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

        case RHI::BlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case RHI::BlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        }

        return {};
    }

    GraphicsPipeline::GraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& desc)
        : Pipeline(device, desc)
        , desc(desc)
    {
        auto srgManager = device->GetShaderResourceManager();

        SetupColorBlendState();
    }

    GraphicsPipeline::~GraphicsPipeline()
    {

    }

    void GraphicsPipeline::Create(RenderPass* renderPass, int subpass)
    {
        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        createInfo.renderPass = renderPass->GetHandle();
        createInfo.subpass = subpass;

        const auto& renderPassDesc = renderPass->GetDescriptor();


        for (const auto& subpassAttachment : renderPassDesc.subpasses[subpass].renderTargetAttachments)
        {
            const auto& attachmentBinding = renderPassDesc.attachments[subpassAttachment.attachmentIndex];
            
        }
    }

    void GraphicsPipeline::SetupColorBlendState()
    {
        colorBlendState = {};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        
        colorBlendState.logicOpEnable = VK_FALSE;
        colorBlendState.logicOp = VK_LOGIC_OP_CLEAR;

        colorBlendAttachments.Clear();

        for (int i = 0; i < desc.colorBlends.GetSize(); i++)
        {
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.blendEnable = desc.colorBlends[i].blendEnable ? VK_TRUE : VK_FALSE;
            colorBlendAttachment.colorBlendOp = RHIBlendOpToVk(desc.colorBlends[i].colorBlendOp);
            colorBlendAttachment.srcColorBlendFactor = RHIBlendFactorToVk(desc.colorBlends[i].srcColorBlend);
            colorBlendAttachment.dstColorBlendFactor = RHIBlendFactorToVk(desc.colorBlends[i].dstColorBlend);
            colorBlendAttachment.alphaBlendOp = RHIBlendOpToVk(desc.colorBlends[i].alphaBlendOp);
            colorBlendAttachment.srcAlphaBlendFactor = RHIBlendFactorToVk(desc.colorBlends[i].srcAlphaBlend);
            colorBlendAttachment.dstAlphaBlendFactor = RHIBlendFactorToVk(desc.colorBlends[i].dstAlphaBlend);
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

            colorBlendAttachments.Add(colorBlendAttachment);
        }
    }

} // namespace CE::Vulkan
