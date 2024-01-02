#pragma once

namespace CE::Vulkan
{
    
    class VULKANRHI_API VulkanSampler : public RHI::Sampler
    {
    public:
        VulkanSampler(VulkanDevice* device, const RHI::SamplerDescriptor& samplerDesc);
        virtual ~VulkanSampler();
        
        virtual void* GetHandle() override
        {
            return sampler;
        }
        
        VkSampler GetVkHandle()
        {
            return sampler;
        }

		VkFilter ToVkFilter(RHI::FilterMode filterMode);
        
    private:
        
        VulkanDevice* device = nullptr;
        VkSampler sampler{};
        Color borderColor{};
    };

} // namespace CE
