#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    PipelineLayout::~PipelineLayout()
    {
        if (pipelineLayout)
        {
            vkDestroyPipelineLayout(device->GetHandle(), pipelineLayout, nullptr);
            pipelineLayout = nullptr;
        }

        for (VkDescriptorSetLayout setLayout : setLayouts)
        {
            if (setLayout == emptySetLayout)
                continue;

            vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, nullptr);
        }
        setLayouts.Clear();

        if (emptySetLayout)
        {
            vkDestroyDescriptorSetLayout(device->GetHandle(), emptySetLayout, nullptr);
            emptySetLayout = nullptr;
        }
    }

    bool PipelineLayout::IsCompatibleWith(PipelineLayout* other)
    {
        for (const auto& [set, bindingList] : setLayoutBindingsMap)
        {
            if (!other->setLayoutBindingsMap.KeyExists(set))
                return false;
            if (bindingList.GetSize() != other->setLayoutBindingsMap[set].GetSize())
                return false;

            const auto& otherBindingList = other->setLayoutBindingsMap[set];

            for (int i = 0; i < bindingList.GetSize(); i++)
            {
                if (bindingList[i].binding != otherBindingList[i].binding)
                    return false;
                if (bindingList[i].descriptorCount != otherBindingList[i].descriptorCount)
                    return false;
                if (bindingList[i].descriptorType != otherBindingList[i].descriptorType)
                    return false;
                if (bindingList[i].stageFlags != otherBindingList[i].stageFlags)
                    return false;
                if (bindingList[i].pImmutableSamplers != nullptr &&
                    otherBindingList[i].pImmutableSamplers == nullptr)
                    return false;
                if (bindingList[i].pImmutableSamplers == nullptr &&
                    otherBindingList[i].pImmutableSamplers != nullptr)
                    return false;
            }
        }

        return true;
    }

    Pipeline::Pipeline(VulkanDevice* device, const PipelineDescriptor& desc) : RHI::RHIResource(RHI::ResourceType::Pipeline), name(desc.name)
    {
        this->device = device;
        
        for (const auto& layout : desc.srgLayouts)
        {
            srgLayouts[layout.srgType] = layout;
        }

        auto srgManager = device->GetShaderResourceManager();
        setLayoutBindingsMap = {};
        setLayoutBindingsByName = {};
        setLayouts = {};
        VkResult result;
        int lowestSetNumber = RHI::Limits::Pipeline::MaxShaderResourceGroupCount;
        int highestSetNumber = -1;

        {
            VkDescriptorSetLayoutCreateInfo emptySetCI{};
            emptySetCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            emptySetCI.bindingCount = 0;
            emptySetCI.pBindings = nullptr;

            vkCreateDescriptorSetLayout(device->GetHandle(), &emptySetCI, nullptr, &emptySetLayout);
        }

        for (const RHI::ShaderResourceGroupLayout& srgLayout : desc.srgLayouts)
        {
            int setNumber = srgManager->GetDescriptorSetNumber(srgLayout.srgType);

            if (setNumber < lowestSetNumber)
                lowestSetNumber = setNumber;
            if (setNumber > highestSetNumber)
                highestSetNumber = setNumber;
        }
        
        for (int setNumber = 0; setNumber <= highestSetNumber; setNumber++)
        {
            bool found = false;

            for (const RHI::ShaderResourceGroupLayout& srgLayout : desc.srgLayouts)
            {
                if (srgManager->GetDescriptorSetNumber(srgLayout.srgType) != setNumber)
                    continue;

                found = true;

                for (const auto& variable : srgLayout.variables)
                {
                    VkDescriptorSetLayoutBinding layoutBinding{};
                    layoutBinding.binding = variable.bindingSlot;
                    layoutBinding.descriptorCount = variable.arrayCount;
                    layoutBinding.stageFlags = 0;
                    if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Vertex))
                    {
                        layoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                    }
                    if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Fragment))
                    {
                        layoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                    }
                    if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Geometry))
                    {
                        layoutBinding.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
                    }
                    if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Tessellation))
                    {
                        layoutBinding.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                    }

                    switch (variable.type)
                    {
                    case RHI::ShaderResourceType::ConstantBuffer:
                        layoutBinding.descriptorType = variable.usesDynamicOffset
                            ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                            : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;
                    case RHI::ShaderResourceType::StructuredBuffer:
                    case RHI::ShaderResourceType::RWStructuredBuffer:
                        layoutBinding.descriptorType = variable.usesDynamicOffset
                            ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
                            : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        break;
                    case RHI::ShaderResourceType::Texture1D:
                    case RHI::ShaderResourceType::Texture2D:
                    case RHI::ShaderResourceType::Texture3D:
                    case RHI::ShaderResourceType::TextureCube:
                        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                        break;
                    case RHI::ShaderResourceType::RWTexture2D:
                    case RHI::ShaderResourceType::RWTexture3D:
                        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                        break;
                    case RHI::ShaderResourceType::SamplerState:
                        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                        break;
                    case RHI::ShaderResourceType::SubpassInput:
                        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                        break;
                    default:
                        continue;
                    }

                    setLayoutBindingsMap[setNumber].Add(layoutBinding);

                    setLayoutBindingsByName[variable.name] = layoutBinding;
                }

                // TODO: Implement variable size arrays
                VkDescriptorBindingFlags flags[3];
                flags[0] = 0;
                flags[1] = 0;
                flags[2] = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;

                VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags{};
                bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
                bindingFlags.bindingCount = 3;
                bindingFlags.pBindingFlags = flags;

                VkDescriptorSetLayoutCreateInfo setLayoutCI{};
                setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                setLayoutCI.bindingCount = setLayoutBindingsMap[setNumber].GetSize();
                setLayoutCI.pBindings = setLayoutBindingsMap[setNumber].GetData();

                VkDescriptorSetLayout setLayout = nullptr;
                result = vkCreateDescriptorSetLayout(device->GetHandle(), &setLayoutCI, nullptr, &setLayout);
                if (result == VK_SUCCESS)
                {
                    setLayouts.Add(setLayout);
                }

                break;
            }

            // Empty descriptor sets
            if (!found)
            {
                setLayouts.Add(emptySetLayout);
            }
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCI{};
        pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCI.setLayoutCount = setLayouts.GetSize();
        pipelineLayoutCI.pSetLayouts = setLayouts.GetData();
        pipelineLayoutCI.pushConstantRangeCount = 0;

        result = vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCI, nullptr, &pipelineLayout);
        if (result != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create Vulkan Pipeline Layout!");
        }
    }

    Pipeline::~Pipeline()
    {
        if (pipeline)
        {
            vkDestroyPipeline(device->GetHandle(), pipeline, nullptr);
            pipeline = nullptr;
        }

        if (pipelineCache)
        {
            vkDestroyPipelineCache(device->GetHandle(), pipelineCache, nullptr);
            pipelineCache = nullptr;
        }
    }

} // namespace CE::Vulkan
