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

        for (auto setLayout : setLayouts)
        {
            vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, nullptr);
        }
        setLayouts.Clear();
    }

    Pipeline::Pipeline(VulkanDevice* device, const PipelineDescriptor& desc) : RHI::RHIResource(RHI::ResourceType::Pipeline)
    {
        this->device = device;
        auto srgManager = device->GetShaderResourceManager();
        setLayoutBindingsMap = {};
        setLayouts = {};
        VkResult result = VK_SUCCESS;

        for (const RHI::ShaderResourceGroupLayout& srgLayout : desc.srgLayouts)
        {
            int setNumber = srgManager->GetDescriptorSetNumber(srgLayout.srgType);

            for (const auto& variable : srgLayout.variables)
            {
                VkDescriptorSetLayoutBinding layoutBinding{};
                layoutBinding.binding = variable.bindingSlot;
                layoutBinding.descriptorCount = variable.arrayCount;
                layoutBinding.stageFlags = 0;
                if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Vertex))
                {
                    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                }
                if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Fragment))
                {
                    layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                }
                if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Geometry))
                {
                    layoutBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
                }
                if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Tessellation))
                {
                    layoutBinding.stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
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
            }

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
        }
    }

} // namespace CE::Vulkan
