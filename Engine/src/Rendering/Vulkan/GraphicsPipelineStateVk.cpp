
#include "EngineDefs.h"

#include "GraphicsPipelineStateVk.h"
#include "TypesVk.h"

#include <glm/glm.hpp>

#include <stdexcept>
#include <iostream>
#include <array>

using namespace Vox;

const char* pShaderEntry = "main";

GraphicsPipelineStateVk::GraphicsPipelineStateVk(const GraphicsPipelineStateCreateInfo &createInfo)
{
    m_pDevice = dynamic_cast<DeviceContextVk*>(createInfo.pDevice);
    m_pSwapChain = dynamic_cast<SwapChainVk*>(createInfo.pSwapChain);
    m_pShader = dynamic_cast<ShaderVk*>(createInfo.pShader);
    m_pRenderContext = dynamic_cast<RenderContextVk*>(createInfo.pRenderContext);

    CreateGraphicsPipeline(createInfo);
}

GraphicsPipelineStateVk::~GraphicsPipelineStateVk()
{
    for (int i = 0; i < m_VPUniformBuffer.size(); ++i)
    {
        if (m_VPUniformBuffer[i] != nullptr)
            delete m_VPUniformBuffer[i];
    }
    m_VPUniformBuffer.clear();

    for (int i = 0; i < m_ModelUniformBufferDynamic.size(); ++i)
    {
        if (m_ModelUniformBufferDynamic[i] != nullptr)
            delete m_ModelUniformBufferDynamic[i];
    }
    m_ModelUniformBufferDynamic.clear();

    //vkDestroyDescriptorSetLayout(m_pDevice->GetDevice(), m_DescriptorSetLayout, nullptr);

    vkDestroyPipeline(m_pDevice->GetDevice(), m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_pDevice->GetDevice(), m_PipelineLayout, nullptr);
}

#pragma region Internal API

void GraphicsPipelineStateVk::CreateGraphicsPipeline(const GraphicsPipelineStateCreateInfo& createInfo)
{
    // -- Vertex Input State --
    VkVertexInputBindingDescription vertBindingDesc = {};
    vertBindingDesc.binding = 0;
    vertBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertBindingDesc.stride = createInfo.vertexStructByteSize;

    std::vector<VkVertexInputAttributeDescription> vertexAttribs(createInfo.attributesCount);

    for (int i = 0; i < createInfo.attributesCount; ++i)
    {
        vertexAttribs[i].binding = createInfo.pAttributes[i].binding;
        vertexAttribs[i].location = createInfo.pAttributes[i].location;
        vertexAttribs[i].format = VkFormatFromVertexAttribFormat(createInfo.pAttributes[i].format);
        vertexAttribs[i].offset = createInfo.pAttributes[i].offset;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = 1;
    vertexInputState.pVertexBindingDescriptions = &vertBindingDesc;
    vertexInputState.vertexAttributeDescriptionCount = createInfo.attributesCount;
    vertexInputState.pVertexAttributeDescriptions = vertexAttribs.data();

    // -- Input Assembly State --
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    // -- Viewport State (Dynamic) --
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    // -- Dynamic States --
    VkDynamicState dynamicStateEnables[2] = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStateEnables;

    // -- Rasterizer State --
    VkPipelineRasterizationStateCreateInfo rasterizerState = {};
    rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerState.depthClampEnable = VK_FALSE;   // Set if fragments beyond far plan are clipped (default) or clamped to plane
    rasterizerState.rasterizerDiscardEnable = VK_FALSE; // Stops the pipeline here if true. Useful only if you don't need pixel output
    rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;  // How to handle filling points between vertices
    rasterizerState.lineWidth = 1.0f;                    // How think the lines should be when drawn
    rasterizerState.cullMode = VK_CULL_MODE_NONE;    // Cull the back face of triangles
    rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE; // Front face of triangle is the one whose indices are in clockwise direction
    rasterizerState.depthBiasEnable = VK_FALSE;

    // -- Multisampling State --
    VkPipelineMultisampleStateCreateInfo multisamplingState = {};
    multisamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingState.sampleShadingEnable = VK_FALSE;  // Disable multisample shading for now
    multisamplingState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // No. of samples to use per fragment

    // -- Blending State --
    // Blend attachment state
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;  // Enable blending

    // outColor = srcAlpha * srcColor + (1 - srcAlpha) * srcColor
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    // outAlpha = srcAlpha * 1 + dstAlpha * 0
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.logicOpEnable = VK_FALSE; // Should we use logical operations instead of calculations
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachment;

    // -- Descriptor Set Layouts --
    /*VkDescriptorSetLayoutBinding vpLayoutBinding = {};
    vpLayoutBinding.binding = 0;
    vpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vpLayoutBinding.descriptorCount = 1;
    vpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vpLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding modelLayoutBinding = {};
    modelLayoutBinding.binding = 1;
    modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    modelLayoutBinding.descriptorCount = 1;
    modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    modelLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> layoutBindings{ vpLayoutBinding, modelLayoutBinding };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size()); // No. of binding infos
    descriptorSetLayoutInfo.pBindings = layoutBindings.data();                           // Array of binding infos

    auto result = vkCreateDescriptorSetLayout(m_pDevice->GetDevice(), &descriptorSetLayoutInfo, nullptr, &m_DescriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Descriptor Set Layout!");
    }*/

    // -- Descriptor Set Layouts --
    std::array<VkDescriptorSetLayout, 1> descriptorSetLayouts = {m_pRenderContext->GetGlobalDescriptorSetLayout()};

    // -- Push Constants --
    VkPushConstantRange pushConstant = {};
    pushConstant.offset = 0;  // The beginning of this range
    pushConstant.size = sizeof(glm::mat4); // The size of this range ; Only passing a mat4 to push constants for now
    pushConstant.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS; // Accessible to all graphics stages (vertex, fragment, etc)

    // -- Pipeline Layout --
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (m_PipelineLayout != nullptr)
    {
        vkDestroyPipelineLayout(m_pDevice->GetDevice(), m_PipelineLayout, nullptr);
        m_PipelineLayout = nullptr;
    }

    // Create Pipeline Layout
    auto result = vkCreatePipelineLayout(m_pDevice->GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Pipeline Layout!");
    }

    // -- Depth Stencil --
    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS; // Only draw if Z depth of new pixel is less than current
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.stencilTestEnable = VK_FALSE;   // We aren't using stencil at this moment

    // -- Shader Stages --
    auto shaderVariant = m_pShader->GetCurrentVariant();
    // Vertex Shader
    VkPipelineShaderStageCreateInfo vertexShaderStage = {};
    vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStage.module = shaderVariant->GetVertexModule();
    vertexShaderStage.pName = m_pShader->GetVertEntryPoint();

    // Fragment Shader
    VkPipelineShaderStageCreateInfo fragmentShaderStage = {};
    fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStage.module = shaderVariant->GetFragmentModule();
    fragmentShaderStage.pName = m_pShader->GetFragEntryPoint();

    VkPipelineShaderStageCreateInfo shaderStages[2] = { vertexShaderStage, fragmentShaderStage };

    // -- Graphics Pipeline Creation --
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2; // No. of shader stages (vertex & fragment)
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.pRasterizationState = &rasterizerState;
    pipelineInfo.pMultisampleState = &multisamplingState;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = m_pSwapChain->GetRenderPass();
    pipelineInfo.subpass = 0; // You can use only 1 subpass per pipeline. Use separate pipeline for different subpasses

    // Pipeline derivatives : Can create multiple pipelines that rely on one another for optimization
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Existing pipeline to derive from
    pipelineInfo.basePipelineIndex = -1; // OR index of pipeline being created to derive from

    if (m_Pipeline != nullptr)
    {
        vkDestroyPipeline(m_pDevice->GetDevice(), m_Pipeline, nullptr);
        m_Pipeline = nullptr;
    }

    result = vkCreateGraphicsPipelines(m_pDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Graphics Pipeline!");
    }
}

/*
void GraphicsPipelineStateVk::CreateDynamicUniformBuffer(Uint64 bufferSize, BufferData& initialData, uint64_t alignment)
{
    auto maxSimultaneousFrames = m_pSwapChain->GetMaxSimultaneousFrameDraws();
    m_DynamicUniformBufferAlignment = alignment;

    // -- Dynamic Uniform Buffer --
    m_ModelUniformBufferDynamic.resize(maxSimultaneousFrames);

    for (int i = 0; i < maxSimultaneousFrames; ++i)
    {
        BufferCreateInfo bufferInfo = {};
        bufferInfo.size = bufferSize;
        bufferInfo.pName = "Uniform Buffer";
        bufferInfo.bindFlags = BIND_UNIFORM_BUFFER;
        bufferInfo.allocType = BUFFER_MEM_CPU_TO_GPU;
        bufferInfo.transferFlags = BUFFER_TRANSFER_NONE;
        bufferInfo.optimizationFlags = BUFFER_OPTIMIZE_UPDATE_REGULAR_BIT;

        auto* buffer = new BufferVk(bufferInfo, initialData, m_pDevice);
        m_ModelUniformBufferDynamic[i] = buffer;
    }

    // -- Descriptor Sets --

    for (int i = 0; i < m_ModelUniformBufferDynamic.size(); ++i)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_ModelUniformBufferDynamic[i]->GetBuffer(); // Buffer to get data from
        bufferInfo.offset = 0;              // Position of start of data
        bufferInfo.range = alignment;      // Size of the data

        // Data about connection between binding and buffer. It uses the buffer to write to the descriptor set
        VkWriteDescriptorSet setWrite = {};
        setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrite.dstSet = m_DescriptorSets[i];  // Descriptor set to update
        setWrite.dstBinding = 1;        // Binding to update (matches with binding on layout/shader)
        setWrite.dstArrayElement = 0;   // Index in array to update
        setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;  // Dynamic Uniform Buffer
        setWrite.descriptorCount = 1;  // No. of descriptors to update
        setWrite.pBufferInfo = &bufferInfo;  // Info about buffer data to bind
        setWrite.pImageInfo = nullptr;
        setWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(m_pDevice->GetDevice(), 1, &setWrite, 0, nullptr);
    }
}
*/

VkFormat GraphicsPipelineStateVk::VkFormatFromVertexAttribFormat(VertexAttribFormat &attribFormat)
{
    switch (attribFormat)
    {
        case VERTEX_ATTRIB_FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case VERTEX_ATTRIB_FLOAT2:
            return VK_FORMAT_R32G32_SFLOAT;
        case VERTEX_ATTRIB_FLOAT3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case VERTEX_ATTRIB_FLOAT4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case VERTEX_ATTRIB_HALF:
            return VK_FORMAT_R16_SFLOAT;
        case VERTEX_ATTRIB_HALF2:
            return VK_FORMAT_R16G16_SFLOAT;
        case VERTEX_ATTRIB_HALF3:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case VERTEX_ATTRIB_HALF4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
    }
    throw std::runtime_error("ERROR: Invalid VertexAttribFormat used in a Graphics Pipeline!");
}

#pragma endregion

