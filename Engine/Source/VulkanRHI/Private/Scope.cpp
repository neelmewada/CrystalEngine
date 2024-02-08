#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
    
	Scope::Scope(VulkanDevice* device, const RHI::ScopeDescriptor& desc) : Super(desc), device(device)
	{

	}

	Scope::~Scope()
	{
		vkDeviceWaitIdle(device->GetHandle());

		for (int i = 0; i < commandListsByFamilyIndexPerImage.GetSize(); i++)
		{
			for (int j = 0; j < commandListsByFamilyIndexPerImage[i].GetSize(); j++)
			{
				delete commandListsByFamilyIndexPerImage[i][j];
			}
			commandListsByFamilyIndexPerImage[i].Clear();
		}
		commandListsByFamilyIndexPerImage.Clear();

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

			VkResult result = VK_SUCCESS;

			//VkSemaphore semaphore = nullptr;
			//result = vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &semaphore);
			if (result != VK_SUCCESS)
			{
				continue;
			}
			//renderFinishedSemaphores.Add(semaphore);

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
			signalSemaphores.Add({});
			signalSemaphoresByConsumerScope.Add({});

			// 1 signal semaphore for 1 consumer
			for (auto consumerRhiScope : consumers)
			{
				Vulkan::Scope* consumerScope = (Vulkan::Scope*)consumerRhiScope;
				while (consumerScope->prevSubPass != nullptr)
				{
					consumerScope = (Vulkan::Scope*)consumerScope->prevSubPass;
				}

				if (signalSemaphoresByConsumerScope[i].KeyExists(consumerScope->id))
					continue;

				VkSemaphore signalSemaphore = nullptr;
				vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &signalSemaphore);
				signalSemaphores[i].Add(signalSemaphore);
				signalSemaphoresByConsumerScope[i].Add(consumerScope->id, signalSemaphore);
			}
			// No consumers exist, then just signal 1 semaphore
			if (consumers.IsEmpty())
			{
				VkSemaphore signalSemaphore = nullptr;
				vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &signalSemaphore);
				signalSemaphores[i].Add(signalSemaphore);
			}
		}

		if (prevSubPass == nullptr && nextSubPass != nullptr)
		{
			// TODO: RenderPass with multiple subpasses
			Vulkan::Scope* next = this;
			this->subpassIndex = 0;
			int i = 0;

			while (next != nullptr)
			{
				// Assign appropriate subpass indices
				next->subpassIndex = i++;
				next = (Vulkan::Scope*)next->nextSubPass;
			}

			RenderPassCache* rpCache = device->GetRenderPassCache();
			RenderPass::Descriptor descriptor{};
			RenderPass::BuildDescriptor(this, descriptor);
			renderPass = rpCache->FindOrCreate(descriptor);

			next = this;

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

		for (int i = 0; i < signalSemaphores.GetSize(); i++)
		{
			for (auto semaphore : signalSemaphores[i])
			{
				vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
			}
			signalSemaphores[i].Clear();
			signalSemaphoresByConsumerScope[i].Clear();
		}
		signalSemaphores.Clear();
		signalSemaphoresByConsumerScope.Clear();

		//for (VkSemaphore semaphore : renderFinishedSemaphores)
		//{
		//	vkDestroySemaphore(device->GetHandle(), semaphore, nullptr);
		//}
		//renderFinishedSemaphores.Clear();

		for (VkFence fence : renderFinishedFences)
		{
			vkDestroyFence(device->GetHandle(), fence, nullptr);
		}
		renderFinishedFences.Clear();
	}

} // namespace CE::Vulkan
