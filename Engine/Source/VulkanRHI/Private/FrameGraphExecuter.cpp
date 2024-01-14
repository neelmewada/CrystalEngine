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
		//Vulkan::Scope* presentingScope = (Vulkan::Scope*)frameGraph->presentingScope;
		//auto swapChain = (Vulkan::SwapChain*)frameGraph->presentSwapChain;
		
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

		VkCommandBufferBeginInfo cmdBeginInfo{};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandList->commandBuffer, &cmdBeginInfo);
		{

		}
		vkEndCommandBuffer(commandList->commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		

		return result == VK_SUCCESS;
	}
    
} // namespace CE::Vulkan
