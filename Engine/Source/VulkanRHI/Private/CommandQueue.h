#pragma once

#include "VulkanRHI.h"
#include "vulkan/vulkan.h"

namespace CE::Vulkan
{
    class VulkanDevice;

    class CommandQueue : public RHI::CommandQueue
    {
    public:
        CommandQueue(VulkanDevice* device, u32 familyIndex, u32 queueIndex, RHI::HardwareQueueClassMask queueMask, VkQueue queue, bool presentSupported);
        virtual ~CommandQueue();

		inline VkQueue GetHandle() const
        {
            return queue;
        }

		inline u32 GetFamilyIndex() const
        {
            return familyIndex;
        }

        inline u32 GetQueueIndex() const
        {
            return queueIndex;
        }
		
		inline bool IsPresentSupported() const
		{
			return presentSupported;
		}

		inline VkCommandPool GetCommandPool() const
		{
			return commandPool;
		}

    private:
        VulkanDevice* device;
        u32 familyIndex;
        u32 queueIndex;
        VkQueue queue;
		bool presentSupported = false;
		VkCommandPool commandPool = nullptr;

		friend class VulkanDevice;
    };
    
} // namespace CE
