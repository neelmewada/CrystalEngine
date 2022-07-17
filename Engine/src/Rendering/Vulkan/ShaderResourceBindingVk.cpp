
#include "ShaderResourceBindingVk.h"

#include "TextureVk.h"

#include <iostream>

using namespace Vox;

ShaderResourceVariableVk::ShaderResourceVariableVk(const ShaderResourceVariableVkCreateInfo& createInfo,
                                                   IShaderResourceBindingCallbacks* pReceiver)
{
    m_Set = createInfo.set;
    m_Binding = createInfo.binding;
    m_ResourceVariableType = createInfo.resourceType;
    m_DescriptorCount = createInfo.descriptorCount;
    m_Name = createInfo.name;
    m_pReceiver = pReceiver;
    m_BindingRef = createInfo.bindingRef;
}

ShaderResourceVariableVk::~ShaderResourceVariableVk()
{

}

void ShaderResourceVariableVk::Set(IDeviceObject* pObject)
{
    m_pReceiver->BindDeviceObject(m_BindingRef, pObject, m_Set, m_Binding,
                                  m_DescriptorCount, m_ResourceVariableType);
}

#pragma region ShaderResourceBindingVk

ShaderResourceBindingVk::ShaderResourceBindingVk(const ShaderResourceBindingVkCreateInfo& createInfo,
                                                 const std::vector<ShaderResourceVariableDefinition>& variableDefinitions,
                                                 const std::vector<VkDescriptorSetLayout>& setLayouts,
                                                 DeviceContextVk* pDevice, RenderContextVk* pRenderCtx)
{
    m_MaxSimultaneousFrames = createInfo.maxSimultaneousFrames;
    m_FirstSet = createInfo.firstSet;
    m_SetCount = createInfo.setCount;
    m_pDevice = pDevice;

    // -- Descriptor Pool --
    VkDescriptorPoolSize sizes[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 4},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 4},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
    };
    for (int i = 0; i < _countof(sizes); ++i)
    {
        sizes[i].descriptorCount *= m_MaxSimultaneousFrames;
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // Fast allocation
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    if (m_BindingFrequency != RESOURCE_BINDING_FREQUENCY_STATIC) // Allow re-binding only for non-static resources
        poolInfo.flags |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.poolSizeCount = _countof(sizes);
    poolInfo.pPoolSizes = sizes;
    poolInfo.maxSets = m_SetCount * m_MaxSimultaneousFrames;

    VK_ASSERT(vkCreateDescriptorPool(m_pDevice->GetDevice(), &poolInfo, nullptr, &m_DescriptorPool),
              "Failed to create Descriptor Pool for Graphics Pipeline");

    VkDescriptorSetAllocateInfo descriptorSetInfo = {};
    descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetInfo.descriptorPool = m_DescriptorPool;
    descriptorSetInfo.descriptorSetCount = static_cast<uint32_t>(setLayouts.size());
    descriptorSetInfo.pSetLayouts = setLayouts.data();

    m_DescriptorSets.resize(descriptorSetInfo.descriptorSetCount);

    VK_ASSERT(vkAllocateDescriptorSets(m_pDevice->GetDevice(), &descriptorSetInfo, m_DescriptorSets.data()),
              "Failed to allocate Static Descriptor Set for Graphics Pipeline!");

    for (int i = 0; i < variableDefinitions.size(); ++i)
    {
        const auto& varDef = variableDefinitions[i];
        if (varDef.name.empty()) continue;

        ShaderResourceVariableVkCreateInfo varInfo = {};
        varInfo.name = varDef.name;
        varInfo.descriptorCount = !varDef.isArray ? 1 : varDef.members.size();
        varInfo.set = varDef.set;
        varInfo.binding = varDef.binding;
        varInfo.resourceType = varDef.type;
        varInfo.bindingRef = this;

        auto* variableBinding = new ShaderResourceVariableVk(varInfo, this);
        m_VariableBindings.push_back(variableBinding);
    }
}

ShaderResourceBindingVk::~ShaderResourceBindingVk()
{
    for (int i = 0; i < m_VariableBindings.size(); ++i)
    {
        delete m_VariableBindings[i];
    }
    m_VariableBindings.clear();

    vkDestroyDescriptorPool(m_pDevice->GetDevice(), m_DescriptorPool, nullptr);
}

IShaderResourceVariable* ShaderResourceBindingVk::GetVariableByName(const char* pName)
{
    for (int i = 0; i < m_VariableBindings.size(); ++i)
    {
        if (m_VariableBindings[i]->m_Name == pName)
            return m_VariableBindings[i];
    }

    return nullptr;
}

void ShaderResourceBindingVk::CmdBindDescriptorSets(VkCommandBuffer commandBuffer, int currentFrame)
{
    Uint32 offset = 0;

    auto descriptorSetCountPerFrame = m_DescriptorSets.size() / m_MaxSimultaneousFrames;

    auto currentPipeline = m_pRenderContext->GetBoundGraphicsPipeline();

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->GetPipelineLayout(), m_FirstSet,
                            m_SetCount,
                            &*(m_DescriptorSets.begin() + currentFrame * descriptorSetCountPerFrame),
                            1,&offset);
}

void ShaderResourceBindingVk::BindDeviceObject(IShaderResourceBinding* resourceBinding, IDeviceObject* pDeviceObject,
                                               Uint32 set, Uint32 binding, Uint32 descriptorCount,
                                               ShaderResourceVariableType resourceType)
{
    auto currentPipeline = m_pRenderContext->GetBoundGraphicsPipeline();

    auto resourceVariableDesc = currentPipeline->m_ShaderResourceVariables[{set, binding}];
    auto flags = resourceVariableDesc.flags;

    bool dynamicOffsetFlag = flags & SHADER_RESOURCE_VARIABLE_DYNAMIC_OFFSET_BIT;

    VkDescriptorBufferInfo bufferInfo = {};
    if (pDeviceObject->GetDeviceObjectType() == IDeviceObject::DEVICE_OBJECT_BUFFER &&
        (resourceType == ::UniformBuffer || resourceType == ::StorageBuffer))
    {
        auto buffer = dynamic_cast<BufferVk*>(pDeviceObject);
        bufferInfo.buffer = buffer->GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = buffer->GetBufferSize();
        VOX_ASSERT(!dynamicOffsetFlag || (dynamicOffsetFlag && buffer->IsDynamicOffset()),
                   "ERROR: If a shader resource variable is marked as Dynamic Offset, the buffer should be Dynamic too and vice versa!");

        if (dynamicOffsetFlag && buffer->IsDynamicOffset())
        {
            bufferInfo.range = buffer->GetStructureByteStride();
        }
    }

    VkDescriptorImageInfo imageInfo = {};
    if (resourceType == ::SampledImage2D || resourceType == ::SampledImageCube)
    {
        TextureViewVk* textureView = nullptr;
        if (pDeviceObject->GetDeviceObjectType() == IDeviceObject::DEVICE_OBJECT_TEXTURE_VIEW)
            textureView = dynamic_cast<TextureViewVk*>(pDeviceObject);
        else if (pDeviceObject->GetDeviceObjectType() == IDeviceObject::DEVICE_OBJECT_TEXTURE)
            textureView = dynamic_cast<TextureVk*>(pDeviceObject)->GetDefaultViewInternal();
        else throw std::runtime_error("ERROR: Resource type is a sampled image, but the passed Device Object is not of texture view type!");
        imageInfo.imageView = textureView->GetImageView();
        imageInfo.imageLayout = textureView->GetImageLayout();
        if (currentPipeline->m_ImmutableSamplers.count(resourceVariableDesc.pVariableName) == 0)
        {
            VOX_ASSERT(textureView->GetSampler() != nullptr,
                       std::string() + "No sampler found for texture named " + resourceVariableDesc.pVariableName + "\n" +
                       "If an immutable sampler description is not passed to Graphics Pipeline, a dynamic sampler should exist in the Texture View!");
            imageInfo.sampler = textureView->GetSampler();
        }
    }

    vkQueueWaitIdle(m_pDevice->GetGraphicsQueue());

    auto maxDescriptorSetCount = m_DescriptorSets.size() / m_MaxSimultaneousFrames;

    for (int frameIdx = 0; frameIdx < m_MaxSimultaneousFrames; ++frameIdx)
    {
        for (int i = 0; i < maxDescriptorSetCount; ++i)
        {
            auto setIdx = i * frameIdx;

            VOX_ASSERT(set < maxDescriptorSetCount || m_DescriptorSets[setIdx] == nullptr,
                       "Failed to bind shader resource! Descriptor Set No. " + std::to_string(set));

            auto descriptorSet = m_DescriptorSets[setIdx];

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
                setWrite.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(m_pDevice->GetDevice(), 1, &setWrite, 0, nullptr);
        }
    }
}

#pragma endregion
