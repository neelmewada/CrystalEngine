#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	Scope::Scope(VulkanDevice* device, const RHI::ScopeDescriptor& desc) : Super(desc), device(device)
	{

	}

	Scope::~Scope()
	{
		DestroySyncObjects();
	}

	bool Scope::CompileInternal(const FrameGraphCompileRequest& compileRequest)
	{
		DestroySyncObjects();

		auto frameGraph = compileRequest.frameGraph;
		SwapChain* swapChain = (Vulkan::SwapChain*)frameGraph->GetSwapChain();
		
		u32 numFrames = Math::Clamp<u32>(compileRequest.numFramesInFlight, 1, RHI::Limits::Pipeline::MaxFramesInFlight);
		if (swapChain != nullptr)
		{
			numFrames = swapChain->GetImageCount();
		}

		if (PresentsSwapChain())
		{
			for (int i = 0; i < numFrames; i++)
			{
				VkSemaphoreCreateInfo semaphoreCI{};
				semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				
				VkSemaphore semaphore = nullptr;
				auto result = vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &semaphore);
				if (result != VK_SUCCESS)
				{
					continue;
				}

				imageAquiredSemaphores.Add(semaphore);
			}
		}

		for (int i = 0; i < numFrames; i++)
		{
			VkSemaphoreCreateInfo semaphoreCI{};
			semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkSemaphore semaphore = nullptr;
			auto result = vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &semaphore);
			if (result != VK_SUCCESS)
			{
				continue;
			}

			renderFinishedSemaphores.Add(semaphore);

			VkFenceCreateInfo fenceCI{};
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			
			VkFence fence = nullptr;
			result = vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &fence);
			if (result != VK_SUCCESS)
			{
				continue;
			}

			renderFinishedFences.Add(fence);
		}
	}

	void Scope::DestroySyncObjects()
	{
		for (VkSemaphore semaphore : imageAquiredSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		imageAquiredSemaphores.Clear();

		for (VkSemaphore semaphore : renderFinishedSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		renderFinishedSemaphores.Clear();
	}

} // namespace CE::Vulkan
