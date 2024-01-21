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

    static VkCompareOp RHICompareOpToVk(RHI::CompareOp compareOp)
    {
        switch (compareOp)
        {
        case RHI::CompareOp::Never:
            return VK_COMPARE_OP_NEVER;
        case RHI::CompareOp::Less:
            return VK_COMPARE_OP_LESS;
        case RHI::CompareOp::Equal:
            return VK_COMPARE_OP_EQUAL;
        case RHI::CompareOp::LessOrEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case RHI::CompareOp::Greater:
            return VK_COMPARE_OP_GREATER;
        case RHI::CompareOp::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case RHI::CompareOp::GreaterOrEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case RHI::CompareOp::Always:
            return VK_COMPARE_OP_ALWAYS;
        }
        return {};
    }

    GraphicsPipeline::GraphicsPipeline(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& desc)
        : Pipeline(device, desc)
        , desc(desc)
    {
        auto srgManager = device->GetShaderResourceManager();

        SetupColorBlendState();
        SetupDepthStencilState();
        SetupShaderStages();
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

        // - Color Blend State -

        while (colorBlendAttachments.GetSize() < renderPassDesc.subpasses[subpass].renderTargetAttachments.GetSize())
        {
            colorBlendAttachments.Add(colorBlendAttachments.GetLast());
        }

        colorBlendState.attachmentCount = renderPassDesc.subpasses[subpass].renderTargetAttachments.GetSize();
        colorBlendState.pAttachments = colorBlendAttachments.GetData();

        createInfo.pColorBlendState = &colorBlendState;

        // - Dynamic State -
        VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        createInfo.pDynamicState = &dynamicState;

        // - Depth Stencil -
        createInfo.pDepthStencilState = &depthStencilState;

        // - Shader Stages -
        createInfo.stageCount = shaderStages.GetSize();
        createInfo.pStages = shaderStages.GetData();

        // - Raster State -
        createInfo.pRasterizationState = &rasterState;

        // - Input Assembly -
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        
        createInfo.pInputAssemblyState = &inputAssembly;

        // - Vertex Input -
        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.vertexBindingDescriptionCount = vertexBindingDescriptions.GetSize();
        vertexInputState.pVertexBindingDescriptions = vertexBindingDescriptions.GetData();
        vertexInputState.vertexAttributeDescriptionCount = vertexInputDescriptions.GetSize();
        vertexInputState.pVertexAttributeDescriptions = vertexInputDescriptions.GetData();

        createInfo.pVertexInputState = &vertexInputState;

        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.sampleShadingEnable = VK_FALSE;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleState.minSampleShading = 1.0f;
        multisampleState.pSampleMask = nullptr;
        multisampleState.alphaToCoverageEnable = VK_FALSE;
        multisampleState.alphaToOneEnable = VK_FALSE;

        createInfo.pMultisampleState = &multisampleState;

        createInfo.layout = pipelineLayout;

        
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

        if (colorBlendAttachments.IsEmpty())
        {
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_TRUE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

            colorBlendAttachments.Add(colorBlendAttachment);
        }
    }

    void GraphicsPipeline::SetupDepthStencilState()
    {
        depthStencilState = {};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        depthStencilState.depthCompareOp = RHICompareOpToVk(desc.depthStencil.compareOp);
        depthStencilState.depthTestEnable = desc.depthStencil.depthTestEnable ? VK_TRUE : VK_FALSE;
        depthStencilState.depthWriteEnable = desc.depthStencil.depthWriteEnable ? VK_TRUE : VK_FALSE;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.maxDepthBounds = 1.0f;
        depthStencilState.minDepthBounds = 0.0f;
        depthStencilState.back = {};
        depthStencilState.front = {};
        depthStencilState.stencilTestEnable = desc.depthStencil.stencilEnable ? VK_TRUE : VK_FALSE;
    }

    void GraphicsPipeline::SetupShaderStages()
    {
        shaderStages.Clear();

        for (const auto& shaderStageDesc : desc.shaderStages)
        {
            VkPipelineShaderStageCreateInfo shaderStageCI{};
            shaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageCI.module = ((Vulkan::ShaderModule*)shaderStageDesc.shaderModule)->GetHandle();
            
            if (shaderStageDesc.stage == RHI::ShaderStage::Vertex)
                shaderStageCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
            else if (shaderStageDesc.stage == RHI::ShaderStage::Fragment)
                shaderStageCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            else if (shaderStageDesc.stage == RHI::ShaderStage::Geometry)
                shaderStageCI.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            else
                continue;

            shaderStageCI.pName = shaderStageDesc.entryPoint.GetCString();
            
            shaderStages.Add(shaderStageCI);
        }
    }

    void GraphicsPipeline::SetupRasterState()
    {
        rasterState = {};
        rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

        rasterState.depthClampEnable = VK_FALSE;
        rasterState.rasterizerDiscardEnable = VK_FALSE;
        rasterState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterState.lineWidth = 1.0f;

        switch (desc.cullMode)
        {
        case RHI::CullMode::None:
            rasterState.cullMode = VK_CULL_MODE_NONE;
            break;
        case RHI::CullMode::Back:
            rasterState.cullMode = VK_CULL_MODE_BACK_BIT;
            break;
        case RHI::CullMode::Front:
            rasterState.cullMode = VK_CULL_MODE_FRONT_BIT;
            break;
        case RHI::CullMode::All:
            rasterState.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
            break;
        }
        rasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterState.depthBiasEnable = VK_FALSE;
        rasterState.depthBiasClamp = 0;
        rasterState.depthBiasConstantFactor = 0;
        rasterState.depthBiasSlopeFactor = 0;
    }

    void GraphicsPipeline::SetupVertexInputState()
    {
        vertexBindingDescriptions = {};
        vertexInputDescriptions = {};

        for (int i = 0; i < desc.vertexAttribs.GetSize(); i++)
        {
            const auto& vertexAttrib = desc.vertexAttribs[i];

            VkVertexInputAttributeDescription attrib{};
            attrib.binding = 0;
            attrib.location = vertexAttrib.location;
            attrib.offset = vertexAttrib.offset;
            
            switch (vertexAttrib.dataType)
            {
            case VertexAttributeDataType::Float:
                attrib.format = VK_FORMAT_R32_SFLOAT;
                break;
            case VertexAttributeDataType::Float2:
                attrib.format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case VertexAttributeDataType::Float3:
                attrib.format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case VertexAttributeDataType::Float4:
                attrib.format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            case VertexAttributeDataType::Int:
                attrib.format = VK_FORMAT_R32_SINT;
                break;
            case VertexAttributeDataType::Int2:
                attrib.format = VK_FORMAT_R32G32_SINT;
                break;
            case VertexAttributeDataType::Int3:
                attrib.format = VK_FORMAT_R32G32B32_SINT;
                break;
            case VertexAttributeDataType::Int4:
                attrib.format = VK_FORMAT_R32G32B32A32_SINT;
                break;
            }

            vertexInputDescriptions.Add(attrib);
        }

        VkVertexInputBindingDescription inputBinding{};
        inputBinding.binding = 0;
        inputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        inputBinding.stride = desc.vertexStrideInBytes;

        vertexBindingDescriptions.Add(inputBinding);
    }

} // namespace CE::Vulkan
