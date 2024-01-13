#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	FrameGraphExecuter::FrameGraphExecuter(VulkanDevice* device) : device(device)
	{
		
	}

	FrameGraphExecuter::~FrameGraphExecuter()
	{

	}

	bool FrameGraphExecuter::ExecuteInternal(const FrameGraphExecuteRequest& executeRequest)
	{
		FrameGraph* frameGraph = executeRequest.frameGraph;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;

		const Array<RHI::Scope*>& producers = frameGraph->producers;
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		VkResult result = VK_SUCCESS;

		if (swapChain && presentingScope)
		{
			result = vkAcquireNextImageKHR(device->GetHandle(), swapChain->GetHandle(), u64Max, 
				imageAcquiredSemaphores[currentSubmissionIndex],
				imageAcquiredFence[currentSubmissionIndex], &swapChain->currentImageIndex);
		}



		return true;
	}

	bool FrameGraphExecuter::CompileInternal(const FrameGraphCompileRequest& compileRequest)
	{
		DestroySyncObjects();

		FrameGraph* frameGraph = compileRequest.frameGraph;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;
		u32 numFramesInFlight = compileRequest.numFramesInFlight;

		if (swapChain && presentingScope)
		{
			u32 imageCount = swapChain->GetImageCount();
			numFramesInFlight = Math::Min<u32>(imageCount - 1, numFramesInFlight);

			for (int i = 0; i < numFramesInFlight; i++)
			{
				VkSemaphoreCreateInfo semaphoreCI{};
				semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				
				VkSemaphore semaphore = nullptr;
				auto result = vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &semaphore);
				if (result != VK_SUCCESS)
				{
					continue;
				}

				imageAcquiredSemaphores.Add(semaphore);

				VkFenceCreateInfo fenceCI{};
				fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				VkFence fence = nullptr;
				result = vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &fence);
				if (result != VK_SUCCESS)
				{
					continue;
				}
				
				imageAcquiredFence.Add(fence);
			}
		}

		return true;
	}

	void FrameGraphExecuter::DestroySyncObjects()
	{
		for (VkSemaphore semaphore : imageAcquiredSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		imageAcquiredSemaphores.Clear();

		for (VkFence fence : imageAcquiredFence)
		{
			vkDestroyFence(device->GetHandle(), fence, nullptr);
		}
		imageAcquiredFence.Clear();
	}

	bool FrameGraphExecuter::ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope)
	{

		return true;
	}
    
} // namespace CE::Vulkan
