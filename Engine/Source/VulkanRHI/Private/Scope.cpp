#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	Scope::Scope(VulkanDevice* device, const RHI::ScopeDescriptor& desc) : Super(desc), device(device)
	{

	}

	Scope::~Scope()
	{

	}

	bool Scope::CompileInternal(const FrameGraphCompileRequest& compileRequest)
	{
		for (VkSemaphore semaphore : imageAquiredSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		imageAquiredSemaphores.Clear();

		u32 numFramesInFlight = Math::Clamp<u32>(compileRequest.numFramesInFlight, 1, RHI::Limits::Pipeline::MaxSimultaneousFramesInFlight);

		if (PresentsSwapChain())
		{
			for (int i = 0; i < numFramesInFlight; i++)
			{
				VkSemaphoreCreateInfo semaphoreCI{};
				semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				
				VkSemaphore semaphore = nullptr;
				auto result = vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &semaphore);
				if (result != VK_SUCCESS)
				{
					imageAquiredSemaphores.Add(semaphore);
				}
			}
		}
	}

} // namespace CE::Vulkan
