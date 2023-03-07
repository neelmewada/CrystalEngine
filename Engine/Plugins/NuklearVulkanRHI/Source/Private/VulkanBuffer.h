#pragma once

#include "VulkanRHIPrivate.h"

namespace CE
{
    
    class VulkanBuffer : public RHIBuffer
    {
    public:
        VulkanBuffer(VulkanDevice* device, const RHIBufferDesc& desc);
        virtual ~VulkanBuffer();

        virtual RHIBufferBindFlags GetBindFlags() override;

    private:
        Name name{};
        RHIBufferBindFlags bindFlags{};
        RHIBufferUsageFlags usageFlags{};

        u64 bufferSize = 0, structureByteStride = 0;

        VulkanDevice* device = nullptr;
        VkBuffer buffer = nullptr;
    };

} // namespace CE
