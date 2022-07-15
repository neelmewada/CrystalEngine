
#include "EngineDefs.h"
#include "VulkanLimits.h"

#include "GraphicsPipelineStateVk.h"
#include "ShaderResourceBindingVk.h"
#include "TypesVk.h"

#include <glm/glm.hpp>

#include <stdexcept>
#include <iostream>
#include <array>

using namespace Vox;

const char* pShaderEntry = "main";

GraphicsPipelineStateVk::GraphicsPipelineStateVk(const GraphicsPipelineStateCreateInfo &createInfo)
{
    m_Device = dynamic_cast<DeviceContextVk*>(createInfo.pDevice);
    m_SwapChain = dynamic_cast<SwapChainVk*>(createInfo.pSwapChain);
    m_Shader = dynamic_cast<ShaderVk*>(createInfo.pShader);
    m_RenderContext = dynamic_cast<RenderContextVk*>(createInfo.pRenderContext);
    if (createInfo.pName != nullptr)
        m_Name = createInfo.pName;

    CreateDescriptorSets(createInfo);
    CreateStaticResourceBinding(createInfo);
    CreateGraphicsPipeline(createInfo);
}

GraphicsPipelineStateVk::~GraphicsPipelineStateVk()
{
    // -- Graphics Pipeline --
    vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);
    vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);

    // -- Static Resource Binding --
    delete m_StaticBinding;

    // -- Descriptor Sets --
    for (auto& descriptorSetLayout : m_DescriptorSetLayouts)
    {
        vkDestroyDescriptorSetLayout(m_Device->GetDevice(), descriptorSetLayout, nullptr);
    }
    m_DescriptorSetLayouts.clear();

    for (auto& sampler : m_Samplers)
    {
        vkDestroySampler(m_Device->GetDevice(), sampler, nullptr);
    }
    m_Samplers.clear();

    vkDestroyDescriptorPool(m_Device->GetDevice(), m_DescriptorPool, nullptr);
    vkDestroyDescriptorPool(m_Device->GetDevice(), m_StaticDescriptorPool, nullptr);
}

IShaderResourceVariable* GraphicsPipelineStateVk::GetStaticVariableByName(const char* pName)
{
    return m_StaticBinding->GetVariableByName(pName);
}

void GraphicsPipelineStateVk::CmdBindDescriptorSets(VkCommandBuffer commandBuffer)
{
    int currentFrame = m_SwapChain->GetCurrentFrameIndex();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout,0,
                            m_DescriptorSetsPerFrame[currentFrame].size(),
                            m_DescriptorSetsPerFrame[currentFrame].data(),
                            0,nullptr);
}

#pragma region Shader Binding

void GraphicsPipelineStateVk::BindShaderResource(IDeviceObject* pDeviceObject, Uint32 set, Uint32 binding,
                                                 Uint32 descriptorCount, ShaderResourceVariableType resourceType)
{
    auto maxSimultaneousFrames = m_SwapChain->GetMaxSimultaneousFrameDraws();

    auto flags = m_ShaderResourceVariables[{set, binding}].flags;

    bool dynamicOffsetFlag = flags & SHADER_RESOURCE_VARIABLE_DYNAMIC_OFFSET_BIT;

    for (int i = 0; i < maxSimultaneousFrames; ++i)
    {
        VOX_ASSERT(set < m_DescriptorSetsPerFrame[i].size() || m_DescriptorSetsPerFrame[i][set] == nullptr,
                   "Failed to bind shader resource! Descriptor Set No. " + std::to_string(set) +
                   " doesn't exist in Graphics Pipeline " + m_Name);

        auto descriptorSet = m_DescriptorSetsPerFrame[i][set];

        VkDescriptorBufferInfo bufferInfo = {};
        if (pDeviceObject->GetDeviceObjectType() == IDeviceObject::DEVICE_OBJECT_BUFFER &&
            (resourceType == ::UniformBuffer || resourceType == ::StorageBuffer))
        {
            auto buffer = dynamic_cast<BufferVk*>(pDeviceObject);
            bufferInfo.buffer = buffer->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = buffer->GetBufferSize();
            VOX_ASSERT(!dynamicOffsetFlag || (dynamicOffsetFlag && buffer->IsDynamicOffset()),
                       "ERROR: If the shader resource variable is marked as Dynamic Offset, the buffer should be Dynamic too and vice versa!");

            if (dynamicOffsetFlag && buffer->IsDynamicOffset())
            {
                Uint64 numOfElements = buffer->GetBufferSize() / buffer->GetStructureByteStride();
            }
        }

        VkWriteDescriptorSet setWrite = {};
        setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrite.dstSet = descriptorSet;
        setWrite.dstBinding = binding;
        setWrite.dstArrayElement = 0;
        if (resourceType == ::UniformBuffer)
        {
            if (dynamicOffsetFlag) setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            else setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        else if (resourceType == ::StorageBuffer)
        {
            if (dynamicOffsetFlag) setWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            else setWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
        else if (resourceType == ::SampledImage2D)
            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        setWrite.descriptorCount = descriptorCount; // no. of descriptors to use
        if (resourceType == ::UniformBuffer || resourceType == ::StorageBuffer)
            setWrite.pBufferInfo = &bufferInfo;
        else if (resourceType == ::SampledImage2D)
            setWrite.pImageInfo = nullptr;

        vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &setWrite, 0, nullptr);
    }
}

#pragma endregion

#pragma region Internal API

void GraphicsPipelineStateVk::CreateDescriptorSets(const GraphicsPipelineStateCreateInfo& createInfo)
{
    auto maxSimultaneousFrames = m_SwapChain->GetMaxSimultaneousFrameDraws();

    // -- Descriptor Pool --
    VkDescriptorPoolSize sizes[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 4},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 2},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
    };

    for (int i = 0; i < _countof(sizes); ++i)
    {
        sizes[i].descriptorCount *= maxSimultaneousFrames;
    }

    // Static Descriptor Pool
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // Fast allocation
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.poolSizeCount = _countof(sizes);
    poolInfo.pPoolSizes = sizes;
    poolInfo.maxSets = 1 * maxSimultaneousFrames; // Only 1 Static Descriptor Set

    VK_ASSERT(vkCreateDescriptorPool(m_Device->GetDevice(), &poolInfo, nullptr, &m_StaticDescriptorPool),
              "Failed to create Descriptor Pool for Graphics Pipeline");

    // Dynamic Descriptor Pool
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.maxSets = (MAX_BOUND_DESCRIPTOR_SETS - 1) * maxSimultaneousFrames; // Only 3 regular Descriptor Sets

    VK_ASSERT(vkCreateDescriptorPool(m_Device->GetDevice(), &poolInfo, nullptr, &m_DescriptorPool),
              "Failed to create Descriptor Pool for Graphics Pipeline");

    auto& shaderResources = m_Shader->GetCurrentVariant()->GetShaderVariableDefinitions();

    typedef std::vector<VkDescriptorSetLayoutBinding> LayoutBindingCollection;
    std::map<Uint32, LayoutBindingCollection> setBindings{};

    // -- Layout Bindings --
    for (const auto& resource: shaderResources)
    {
        ShaderResourceVariableFlags flags = SHADER_RESOURCE_VARIABLE_NONE_BIT;

        // Find this resource in user passed data from createInfo struct.
        for (int i = 0; i < createInfo.variableCount; ++i)
        {
            const auto& var = createInfo.pResourceVariables[i];
            if (resource.name == var.pVariableName)
            {
                VOX_ASSERT((var.stages & resource.shaderStages) != var.stages,
                           "Shader Stages passed in ShaderResourceVariableDesc doesn't match with the shader stages in Shader!");
                m_ShaderResourceVariables[{resource.set, resource.binding}] = var;
                flags = var.flags;
                break;
            }
        }

        bool dynamicOffset = flags & SHADER_RESOURCE_VARIABLE_DYNAMIC_OFFSET_BIT;

        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = resource.binding;
        if (resource.type == ::UniformBuffer)
        {
            if (dynamicOffset) layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            else layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        else if (resource.type == ::StorageBuffer)
        {
            if (dynamicOffset) layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            else layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
        else if (resource.type == ::SampledImage2D)
            layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        if (resource.isArray)
            layoutBinding.descriptorCount = static_cast<Uint32>(resource.members.size());
        else
            layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = static_cast<VkShaderStageFlags>(resource.shaderStages);
        layoutBinding.pImmutableSamplers = nullptr;
        if (resource.type == ::SampledImage2D)
        {
            // TODO: Create immutable samplers
        }

        if (setBindings.count(resource.set) == 0)
            setBindings.insert(std::pair<Uint32, LayoutBindingCollection>(resource.set, LayoutBindingCollection()));

        setBindings[resource.set].push_back(layoutBinding);
    }

    // -- Descriptor Set Layout --
    m_DescriptorSetLayouts.clear();
    bool set0Used = false;

    for (int set = 0; set < MAX_BOUND_DESCRIPTOR_SETS; ++set) // MAX_BOUND_DESCRIPTOR_SETS = 4
    {
        if (setBindings.count(set) == 0) continue;

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = setBindings.count(set);
        layoutInfo.pBindings = setBindings[set].data();
        if (set > 0) // Set 0 is static
            layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

        VkDescriptorSetLayout setLayout = nullptr;
        VK_ASSERT(vkCreateDescriptorSetLayout(m_Device->GetDevice(), &layoutInfo, nullptr, &setLayout),
                  "Failed to create Descriptor Set Layout!");

        m_DescriptorSetLayouts.push_back(setLayout);
        if (set == 0) set0Used = true;
    }

    // -- Descriptor Sets --
    auto numOfSetsUsed = static_cast<Uint32>(m_DescriptorSetLayouts.size());

    m_DescriptorSetsPerFrame.clear();
    for (int i = 0; i < maxSimultaneousFrames; ++i)
    {
        m_DescriptorSetsPerFrame[i] = std::vector<VkDescriptorSet>(numOfSetsUsed);

        VkDescriptorSetAllocateInfo descriptorSetInfo = {};
        descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetInfo.descriptorPool = m_DescriptorPool;
        descriptorSetInfo.descriptorSetCount = !set0Used ? numOfSetsUsed : numOfSetsUsed - 1;
        descriptorSetInfo.pSetLayouts = !set0Used
                ? m_DescriptorSetLayouts.data()
                : std::vector<VkDescriptorSetLayout>(m_DescriptorSetLayouts.begin() + 1, m_DescriptorSetLayouts.end()).data();

        if (descriptorSetInfo.descriptorSetCount > 0)
            VK_ASSERT(vkAllocateDescriptorSets(m_Device->GetDevice(), &descriptorSetInfo, &*(m_DescriptorSetsPerFrame[i].begin() + 1)),
                      "Failed to allocate Descriptor Sets for Graphics Pipeline!");

        if (set0Used)
        {
            descriptorSetInfo.descriptorPool = m_StaticDescriptorPool;
            descriptorSetInfo.descriptorSetCount = 1;
            descriptorSetInfo.pSetLayouts = &m_DescriptorSetLayouts[0];

            if (descriptorSetInfo.descriptorSetCount > 0)
                VK_ASSERT(vkAllocateDescriptorSets(m_Device->GetDevice(), &descriptorSetInfo, m_DescriptorSetsPerFrame[i].data()),
                          "Failed to allocate Static Descriptor Set for Graphics Pipeline!");
        }
    }
}

void GraphicsPipelineStateVk::CreateStaticResourceBinding(const GraphicsPipelineStateCreateInfo& createInfo)
{
    auto staticVariables = m_Shader->GetCurrentVariant()->GetShaderVariableDefinitions();
    staticVariables.end() = std::remove_if(staticVariables.begin(), staticVariables.end(), [](const ShaderResourceVariableDefinition& item) -> bool {
        return item.set > 0;
    });

    ShaderResourceBindingVkCreateInfo bindingCreateInfo = {
            staticVariables
    };

    m_StaticBinding = new ShaderResourceBindingVk(bindingCreateInfo, this);
}

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

    // -- Pipeline Layout --
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = m_DescriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = m_DescriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (m_PipelineLayout != nullptr)
    {
        vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
        m_PipelineLayout = nullptr;
    }

    // Create Pipeline Layout
    auto result = vkCreatePipelineLayout(m_Device->GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
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
    auto shaderVariant = m_Shader->GetCurrentVariant();
    // Vertex Shader
    VkPipelineShaderStageCreateInfo vertexShaderStage = {};
    vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStage.module = shaderVariant->GetVertexModule();
    vertexShaderStage.pName = m_Shader->GetVertEntryPoint();

    // Fragment Shader
    VkPipelineShaderStageCreateInfo fragmentShaderStage = {};
    fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStage.module = shaderVariant->GetFragmentModule();
    fragmentShaderStage.pName = m_Shader->GetFragEntryPoint();

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
    pipelineInfo.renderPass = m_SwapChain->GetRenderPass();
    pipelineInfo.subpass = 0; // You can use only 1 subpass per pipeline. Use separate pipeline for different subpasses

    // Pipeline derivatives : Can create multiple pipelines that rely on one another for optimization
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Existing pipeline to derive from
    pipelineInfo.basePipelineIndex = -1; // OR index of pipeline being created to derive from

    if (m_Pipeline != nullptr)
    {
        vkDestroyPipeline(m_Device->GetDevice(), m_Pipeline, nullptr);
        m_Pipeline = nullptr;
    }

    result = vkCreateGraphicsPipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Graphics Pipeline!");
    }
}

/*
void GraphicsPipelineStateVk::CreateDynamicUniformBuffer(Uint64 bufferSize, BufferData& initialData, uint64_t alignment)
{
    auto maxSimultaneousFrames = m_SwapChain->GetMaxSimultaneousFrameDraws();
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

        auto* buffer = new BufferVk(bufferInfo, initialData, m_Device);
        m_ModelUniformBufferDynamic[i] = buffer;
    }

    // -- Descriptor Sets --

    for (int i = 0; i < m_ModelUniformBufferDynamic.size(); ++i)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_ModelUniformBufferDynamic[i]->GetBuffer(); // Buffer to get data from
        bufferInfo.offsetInBuffer = 0;              // Position of start of data
        bufferInfo.range = alignment;      // Size of the data

        // Data about connection between binding and buffer. It uses the buffer to write to the descriptor set
        VkWriteDescriptorSet setWrite = {};
        setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrite.dstSet = m_DescriptorSetsPerFrame[i];  // Descriptor set to update
        setWrite.dstBinding = 1;        // Binding to update (matches with binding on layout/shader)
        setWrite.dstArrayElement = 0;   // Index in array to update
        setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;  // Dynamic Uniform Buffer
        setWrite.descriptorCount = 1;  // No. of descriptors to update
        setWrite.pBufferInfo = &bufferInfo;  // Info about buffer data to bind
        setWrite.pImageInfo = nullptr;
        setWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &setWrite, 0, nullptr);
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

