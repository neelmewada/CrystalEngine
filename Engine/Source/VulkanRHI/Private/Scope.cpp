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

		u32 imageCount = Math::Clamp<u32>(compileRequest.numFramesInFlight, 1, RHI::Limits::Pipeline::MaxFramesInFlight);
		u32 numFramesInFlight = imageCount;

		if (swapChain != nullptr)
		{
			imageCount = swapChain->GetImageCount();
			numFramesInFlight = imageCount - 1;
		}

		for (int i = 0; i < imageCount; i++)
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
			fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			
			VkFence fence = nullptr;
			result = vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &fence);
			if (result != VK_SUCCESS)
			{
				continue;
			}

			renderFinishedFences.Add(fence);
		}

		if (prevSubPass == nullptr && nextSubPass != nullptr)
		{
			// TODO: RenderPass with multiple subpasses
		}
	}

	void Scope::DestroySyncObjects()
	{
		for (VkSemaphore semaphore : renderFinishedSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		renderFinishedSemaphores.Clear();

		for (auto fence : renderFinishedFences)
		{
			vkDestroyFence(device->GetHandle(), fence, nullptr);
		}
		renderFinishedFences.Clear();
	}

	void Scope::Execute(const FrameGraphExecuteRequest& executeRequest)
	{
		
	}

} // namespace CE::Vulkan
