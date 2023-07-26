
#include "VulkanRHI.h"
#include "VulkanRHIPrivate.h"
#include "VulkanDevice.h"

#include "VulkanSampler.h"

namespace CE
{
    static VkSamplerAddressMode ToVkSamplerAddressMode(RHI::SamplerAddressMode mode)
    {
        switch (mode)
        {
            case RHI::SAMPLER_ADDRESS_MODE_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case RHI::SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }
        
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
    
    static VkFilter ToVkFilter(RHI::SamplerFilterMode filterMode)
    {
        switch (filterMode)
        {
            case RHI::SAMPLER_FILTER_LINEAR:
                return VK_FILTER_LINEAR;
            case RHI::SAMPLER_FILTER_NEAREST:
                return VK_FILTER_NEAREST;
        }
        return VK_FILTER_LINEAR;
    }

    VulkanSampler::VulkanSampler(VulkanDevice* device, const RHI::SamplerDesc& samplerDesc)
        : device(device), borderColor(samplerDesc.borderColor)
    {
        VkSamplerCreateInfo samplerCI{};
		samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCI.addressModeU = ToVkSamplerAddressMode(samplerDesc.addressModeU);
        samplerCI.addressModeV = ToVkSamplerAddressMode(samplerDesc.addressModeV);
        samplerCI.addressModeW = ToVkSamplerAddressMode(samplerDesc.addressModeW);
        samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        samplerCI.anisotropyEnable = samplerDesc.enableAnisotropy ? VK_TRUE : VK_FALSE;
        samplerCI.maxAnisotropy = samplerDesc.maxAnisotropy;
        samplerCI.minFilter = ToVkFilter(samplerDesc.filterMode);
        samplerCI.magFilter = ToVkFilter(samplerDesc.filterMode);
        
        if (vkCreateSampler(device->GetHandle(), &samplerCI, nullptr, &sampler) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create vulkan sampler");
        }
    }

    VulkanSampler::~VulkanSampler()
    {
        if (sampler != nullptr)
        {
            vkDestroySampler(device->GetHandle(), sampler, nullptr);
        }
    }

} // namespace CE

