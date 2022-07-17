
#include "ShaderResourceBindingVk.h"

#include "TextureVk.h"

#include "GraphicsPipelineStateVk.h"

#include <iostream>
#include <string>

using namespace Vox;

ShaderResourceVariableVk::ShaderResourceVariableVk(const ShaderResourceVariableVkCreateInfo& createInfo,
                                                   IShaderResourceBindingCallbacks* pReceiver)
{
    m_Set = createInfo.set;
    m_Binding = createInfo.binding;
    m_ResourceVariableType = createInfo.resourceType;
    m_DescriptorCount = createInfo.descriptorCount;
    m_Name = createInfo.name;
    m_UsesDynamicOffset = createInfo.usesDynamicOffset;
    m_pReceiver = pReceiver;
}

ShaderResourceVariableVk::~ShaderResourceVariableVk()
{

}

void ShaderResourceVariableVk::Set(IDeviceObject* pObject)
{
    m_pBoundObject = pObject;
    m_pReceiver->BindDeviceObject(pObject, m_Set, m_Binding,
                                  m_DescriptorCount, m_ResourceVariableType);
}

void ShaderResourceVariableVk::Set(IDeviceObject* pObject, Uint32 dynamicOffset)
{
    m_DynamicOffset = dynamicOffset;
    Set(pObject);
    SetDynamicOffset(m_DynamicOffset);
}

void ShaderResourceVariableVk::SetDynamicOffset(Uint32 offset)
{
    if (!m_UsesDynamicOffset) return;
    m_DynamicOffset = offset;
    VOX_ASSERT(m_pBoundObject != nullptr, "ERROR: IShaderResourceVariable::SetDynamicOffset called before any Buffer is bound to it");
    if (m_pBoundObject->GetDeviceObjectType() == IDeviceObject::DEVICE_OBJECT_BUFFER)
    {
        auto* buffer = dynamic_cast<BufferVk*>(m_pBoundObject);
        VOX_ASSERT(buffer != nullptr, "Buffer bound to IShaderResourceVariable, is not of type BufferVk");
        m_DynamicOffset = offset * buffer->GetStructureByteStride();
    }
    m_pReceiver->OnDynamicOffsetUpdated(m_Set, m_Binding, m_DynamicOffset);
}

Uint32 ShaderResourceVariableVk::GetDynamicOffset()
{
    if (!m_UsesDynamicOffset) return 0;
    return m_DynamicOffset;
}

#pragma region ShaderResourceBindingVk

ShaderResourceBindingVk::ShaderResourceBindingVk(const ShaderResourceBindingVkCreateInfo& createInfo,
                                                 const std::vector<ShaderVariableMetaData>& variableMetaData,
                                                 const std::vector<VkDescriptorSetLayout>& setLayouts,
                                                 DeviceContextVk* pDevice, RenderContextVk* pRenderCtx)
{
    m_MaxSimultaneousFrames = createInfo.maxSimultaneousFrames;
    m_FirstSet = createInfo.firstSet;
    m_SetCount = createInfo.setCount;
    m_pDevice = pDevice;
    m_pRenderContext = pRenderCtx;

    m_VariableMetaData.clear();
    for (const auto& item: variableMetaData)
    {
        m_VariableMetaData[{item.set, item.binding}] = item;
    }

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
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Fast allocation
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
    m_DynamicDescriptorCount = 0;
    m_DynamicOffsets.clear();
    m_VariableBindings.clear();

    VK_ASSERT(vkAllocateDescriptorSets(m_pDevice->GetDevice(), &descriptorSetInfo, m_DescriptorSets.data()),
              "Failed to allocate Static Descriptor Set for Graphics Pipeline!");

    for (int i = 0; i < variableMetaData.size(); ++i)
    {
        const auto& varData = variableMetaData[i];
        if (varData.name.empty()) continue;

        ShaderResourceVariableVkCreateInfo varInfo = {};
        varInfo.name = varData.name;
        varInfo.descriptorCount = !varData.isArray ? 1 : varData.arraySize;
        varInfo.set = varData.set;
        varInfo.binding = varData.binding;
        varInfo.resourceType = varData.resourceVarType;
        varInfo.usesDynamicOffset = varData.flags & SHADER_RESOURCE_VARIABLE_DYNAMIC_OFFSET_BIT;

        auto* variableBinding = new ShaderResourceVariableVk(varInfo, this);
        m_VariableBindings.push_back(variableBinding);

        if (varInfo.usesDynamicOffset)
        {
            m_DynamicDescriptorCount++;
            m_VariablePositionInDynamicOffsetsList[{varInfo.set, varInfo.binding}] = m_DynamicDescriptorCount - 1;
        }
    }

    m_DynamicOffsets.resize(m_DynamicDescriptorCount, 0);
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

    std::cerr << "ERROR: Failed to find a variable named " << pName << " in a Shader Resource Binding!" << std::endl;
    return nullptr;
}

void ShaderResourceBindingVk::CmdBindDescriptorSets(VkCommandBuffer commandBuffer, int currentFrame)
{
    Uint32 descriptorSetsStridePerFrame = m_DescriptorSets.size() / m_MaxSimultaneousFrames;

    auto currentPipeline = m_pRenderContext->GetBoundGraphicsPipeline();

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->GetPipelineLayout(), m_FirstSet,
                            m_SetCount,
                            &*(m_DescriptorSets.begin() + currentFrame * descriptorSetsStridePerFrame),
                            m_DynamicDescriptorCount, m_DynamicOffsets.data());
}

void ShaderResourceBindingVk::OnDynamicOffsetUpdated(Uint32 set, Uint32 binding, Uint32 dynamicOffsetValue)
{
    Uint32 index = m_VariablePositionInDynamicOffsetsList[{set, binding}];
    m_DynamicOffsets[index] = dynamicOffsetValue;
}

void ShaderResourceBindingVk::BindDeviceObject(IDeviceObject* pDeviceObject,
                                               Uint32 set, Uint32 binding, Uint32 descriptorCount,
                                               ShaderResourceVariableType resourceType)
{
    auto resourceVariableMetaData = m_VariableMetaData[{set, binding}];
    auto flags = resourceVariableMetaData.flags;

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
        if (!resourceVariableMetaData.immutableSamplerExists)
        {
            VOX_ASSERT(textureView->GetSampler() != nullptr,
                       std::string() + "No sampler found for texture named " + resourceVariableMetaData.name + "\n" +
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

            VOX_ASSERT(m_DescriptorSets[setIdx] != nullptr, "Failed to bind shader resource!");

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
