#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	Scope::Scope(VulkanDevice* device, const RHI::ScopeDescriptor& desc) : Super(desc), device(device)
	{

	}

	Scope::~Scope()
	{
		vkDeviceWaitIdle(device->GetHandle());

        if (renderPass)
        {
            // No need to destroy it. RenderPassCache manages it.
        }
        renderPass = nullptr;

		for (auto frameBuffer : frameBuffers)
		{
			delete frameBuffer;
		}
		frameBuffers.Clear();
        
		DestroySyncObjects();

		delete passShaderResourceGroup;
		passShaderResourceGroup = nullptr;
	}

	bool Scope::CompileInternal(const FrameGraphCompileRequest& compileRequest)
	{
		vkDeviceWaitIdle(device->GetHandle());

		DestroySyncObjects();

		delete passShaderResourceGroup;
		passShaderResourceGroup = nullptr;

		for (auto frameBuffer : frameBuffers)
		{
			delete frameBuffer;
		}
		frameBuffers.Clear();

		auto frameGraph = compileRequest.frameGraph;
		SwapChain* swapChain = (Vulkan::SwapChain*)frameGraph->GetSwapChain();

		u32 imageCount = Math::Clamp<u32>(compileRequest.numFramesInFlight, 1, RHI::Limits::Pipeline::MaxFramesInFlight);

		if (swapChain != nullptr)
		{
			imageCount = swapChain->GetImageCount();
		}

		waitSemaphores.Clear();

		// Render Finished semaphores & fences
		for (int i = 0; i < imageCount; i++)
		{
			waitSemaphores.Add({});

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

			RenderPassCache* rpCache = device->GetRenderPassCache();
			RenderPass::Descriptor descriptor{};
			RenderPass::BuildDescriptor(this, descriptor);
			renderPass = rpCache->FindOrCreate(descriptor);
			this->subpassIndex = 0;
			int i = 0;

			Vulkan::Scope* next = this;

			while (next != nullptr)
			{
				bool foundPipelineLayout = false;
				bool foundSubpassPipelineLayout = false;

				for (RHI::PipelineState* rhiPipelineState : next->usePipelines)
				{
					auto pipelineState = (Vulkan::PipelineState*)rhiPipelineState;
					Pipeline* pipeline = pipelineState->GetPipeline();
					if (!pipeline || pipeline->GetPipelineType() != RHI::PipelineStateType::Graphics)
						continue;
					GraphicsPipeline* graphicsPipeline = (GraphicsPipeline*)pipeline;
					graphicsPipeline->Compile(renderPass, subpassIndex);

					// Setup SRG
					auto pipelineLayout = (Vulkan::PipelineLayout*)rhiPipelineState->GetPipelineLayout();
					if (pipelineLayout != nullptr && !foundPipelineLayout && pipelineLayout->srgLayouts.KeyExists(RHI::SRGType::PerPass))
					{
						foundPipelineLayout = true;
						const RHI::ShaderResourceGroupLayout& srgLayout = pipelineLayout->srgLayouts[RHI::SRGType::PerPass];
						next->passShaderResourceGroup = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
					}

					if (pipelineLayout != nullptr && !foundSubpassPipelineLayout && pipelineLayout->srgLayouts.KeyExists(RHI::SRGType::PerSubPass))
					{
						foundSubpassPipelineLayout = true;
						const RHI::ShaderResourceGroupLayout& srgLayout = pipelineLayout->srgLayouts[RHI::SRGType::PerSubPass];
						next->subpassShaderResourceGroup = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
					}
				}

				next->renderPass = renderPass;
				next->subpassIndex = i++;
				next = (Vulkan::Scope*)next->nextSubPass;
			}
		}
		else if (!IsSubPass())
		{
			// Compile Render Pass
            RenderPassCache* rpCache = device->GetRenderPassCache();
            RenderPass::Descriptor descriptor{};
            RenderPass::BuildDescriptor(this, descriptor);
            renderPass = rpCache->FindOrCreate(descriptor);
			subpassIndex = 0;
			bool foundPipelineLayout = false;
			
			// Pre-Compile Shader Pipelines
			for (RHI::PipelineState* rhiPipelineState : usePipelines)
			{
				auto pipelineState = (Vulkan::PipelineState*)rhiPipelineState;
				Pipeline* pipeline = pipelineState->GetPipeline();
				if (!pipeline || pipeline->GetPipelineType() != RHI::PipelineStateType::Graphics)
					continue;
				GraphicsPipeline* graphicsPipeline = (GraphicsPipeline*)pipeline;
				graphicsPipeline->Compile(renderPass, subpassIndex);

				// Setup SRG
				auto pipelineLayout = (Vulkan::PipelineLayout*)rhiPipelineState->GetPipelineLayout();
				if (pipelineLayout != nullptr && !foundPipelineLayout && pipelineLayout->srgLayouts.KeyExists(RHI::SRGType::PerPass))
				{
					foundPipelineLayout = true;
					const RHI::ShaderResourceGroupLayout& srgLayout = pipelineLayout->srgLayouts[RHI::SRGType::PerPass];
					passShaderResourceGroup = RHI::gDynamicRHI->CreateShaderResourceGroup(srgLayout);
				}
			}
		}
		else
		{
			return true;
		}

		for (int i = 0; i < imageCount; i++)
		{
			frameBuffers.Add(new FrameBuffer(device, this, i));
		}

		return true;
	}

	void Scope::DestroySyncObjects()
	{
		vkDeviceWaitIdle(device->GetHandle());

		for (VkSemaphore semaphore : renderFinishedSemaphores)
		{
			vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		}
		renderFinishedSemaphores.Clear();

		for (VkFence fence : renderFinishedFences)
		{
			vkDestroyFence(device->GetHandle(), fence, nullptr);
		}
		renderFinishedFences.Clear();
	}

} // namespace CE::Vulkan
