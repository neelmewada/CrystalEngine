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
		FrameGraphCompiler* compiler = (Vulkan::FrameGraphCompiler*)executeRequest.compiler;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;

		const Array<RHI::Scope*>& producers = frameGraph->producers;
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		VkResult result = VK_SUCCESS;

		if (swapChain && presentingScope)
		{
			result = vkResetFences(device->GetHandle(), 1, &compiler->imageAcquiredFences[currentSubmissionIndex]);

			result = vkAcquireNextImageKHR(device->GetHandle(), swapChain->GetHandle(), u64Max, 
				compiler->imageAcquiredSemaphores[currentSubmissionIndex],
				compiler->imageAcquiredFences[currentSubmissionIndex], &swapChain->currentImageIndex);
			
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				swapChain->RebuildSwapChain();
				return false; // TODO: Try acquiring image again...
			}

			currentImageIndex = swapChain->currentImageIndex;
		}
		else
		{
			currentImageIndex = currentSubmissionIndex;
		}

		bool success = true;
        
		for (auto rhiScope : frameGraph->producers)
		{
			success = ExecuteScope(executeRequest, (Vulkan::Scope*)rhiScope);
		}

		return success && result == VK_SUCCESS;
	}

	bool FrameGraphExecuter::ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope)
	{
		FrameGraph* frameGraph = executeRequest.frameGraph;
		FrameGraphCompiler* compiler = (Vulkan::FrameGraphCompiler*)executeRequest.compiler;
		Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;
		bool presentRequired = false;
		auto presentQueue = device->GetPresentQueue();
		
		const RHI::FrameGraph::GraphNode& graphNode = frameGraph->nodes[scope->id];
		
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		VkResult result = VK_SUCCESS;

		// Wait for rendering from earlier submission to finish.
		// We cannot record new commands into a command buffer that is currently being executed.
		result = vkWaitForFences(device->GetHandle(), 1, &scope->renderFinishedFences[currentImageIndex], VK_TRUE, u64Max);

		// Manually reset (close) the fence. i.e. put it in unsignalled state
		result = vkResetFences(device->GetHandle(), 1, &scope->renderFinishedFences[currentImageIndex]);
		
		u32 familyIndex = scope->queue->GetFamilyIndex();
		CommandList* commandList = compiler->commandListsByImageIndex[currentImageIndex][familyIndex];

		Array<Scope*> scopeChain{};
		auto scopeInChain = scope;
		while (scopeInChain != nullptr)
		{
			scopeChain.Add(scopeInChain);
			scopeInChain = (Vulkan::Scope*)scopeInChain->next;
		}

		if (scopeChain.Top()->PresentsSwapChain())
			presentRequired = true;

		VkCommandBufferBeginInfo cmdBeginInfo{};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandList->commandBuffer, &cmdBeginInfo);
		{
			
			// Transition to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			if (presentRequired)
			{
				Vulkan::Texture* image = (Vulkan::Texture*)swapChain->GetCurrentImage();

				VkImageMemoryBarrier imageBarrier{};
				imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageBarrier.srcQueueFamilyIndex = image->curFamilyIndex;
				imageBarrier.dstQueueFamilyIndex = presentQueue->GetFamilyIndex();
				imageBarrier.image = image->image;
				imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				//imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				imageBarrier.srcAccessMask = 0;
				imageBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

				imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBarrier.subresourceRange.baseMipLevel = 0;
				imageBarrier.subresourceRange.levelCount = 1;
				imageBarrier.subresourceRange.baseArrayLayer = 0;
				imageBarrier.subresourceRange.layerCount = 1;
				
				vkCmdPipelineBarrier(commandList->commandBuffer,
					//VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageBarrier);
			}
		}
		vkEndCommandBuffer(commandList->commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = scope->waitSemaphores[currentImageIndex].GetSize();
		if (submitInfo.waitSemaphoreCount > 0)
		{
			submitInfo.pWaitSemaphores = scope->waitSemaphores[currentImageIndex].GetData();
			submitInfo.pWaitDstStageMask = scope->waitSemaphoreStageFlags.GetData();
		}
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &scope->renderFinishedSemaphores[currentImageIndex];
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandList->commandBuffer;
		
		result = vkQueueSubmit(scope->queue->GetHandle(), 1, &submitInfo, scope->renderFinishedFences[currentImageIndex]);

		if (presentRequired)
		{
			VkSwapchainKHR swapchainKhr = swapChain->GetHandle();

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapchainKhr;
			presentInfo.pImageIndices = &currentImageIndex;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &scope->renderFinishedSemaphores[currentImageIndex];

			result = vkQueuePresentKHR(presentQueue->GetHandle(), &presentInfo);
		}

		return result == VK_SUCCESS;
	}
    
} // namespace CE::Vulkan
