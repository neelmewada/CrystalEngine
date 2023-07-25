#pragma once

namespace CE
{
    
    class VULKANRHI_API VulkanSampler : public RHI::Sampler
    {
    public:
        VulkanSampler(VulkanDevice* device, const RHI::SamplerDesc& samplerDesc);
        virtual ~VulkanSampler();
        
        virtual void* GetHandle() override
        {
            return sampler;
        }
        
        VkSampler GetVkHandle()
        {
            return sampler;
        }
        
    private:
        
        VulkanDevice* device = nullptr;
        VkSampler sampler{};
        Color borderColor{};
    };

} // namespace CE
