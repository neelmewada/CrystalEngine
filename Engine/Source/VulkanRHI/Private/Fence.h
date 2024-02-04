#pragma once

namespace CE::Vulkan
{
    
    class Fence : public RHI::Fence
    {
    public:
        Fence(VulkanDevice* device, bool signalled = false);

        virtual ~Fence();

        virtual void Reset() override;

        virtual void WaitForFence() override;

        virtual bool IsSignalled() override;

        inline VkFence GetHandle() const { return fence; }

    protected:

        VulkanDevice* device = nullptr;

        VkFence fence = nullptr;

    };

} // namespace CE::Vulkan
