#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	CommandList::CommandList(VulkanDevice* device, VkCommandBuffer commandBuffer, VkCommandBufferLevel level, u32 queueFamilyIndex, VkCommandPool pool)
		: device(device)
		, commandBuffer(commandBuffer)
		, level(level)
		, queueFamilyIndex(queueFamilyIndex)
		, pool(pool)
	{
		srgManager = device->GetShaderResourceManager();
	}

	CommandList::~CommandList()
	{
		vkFreeCommandBuffers(device->GetHandle(), pool, 1, &commandBuffer);
	}

	void CommandList::SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs)
	{
		for (auto rhiSrg : srgs)
		{
			auto srg = (Vulkan::ShaderResourceGroup*)rhiSrg;
			boundSRGs[(int)srg->GetSRGType()] = srg;
		}
	}

	void CommandList::CommitShaderResources()
	{
		if (boundPipeline == nullptr)
			return;

		StaticArray<
			FixedArray<Vulkan::ShaderResourceGroup*, RHI::Limits::Pipeline::MaxShaderResourceGroupCount>,
			RHI::Limits::Pipeline::MaxShaderResourceGroupCount
		> srgsToMerge{};

		for (auto srg : boundSRGs)
		{
			if (!srg)
				continue;

			int setNumber = srg->GetSetNumber();
			srg->Compile();

			srgsToMerge[setNumber].Add(srg);
		}

		for (int setNumber = 0; setNumber < RHI::Limits::Pipeline::MaxShaderResourceGroupCount; setNumber++)
		{
			if (srgsToMerge[setNumber].GetSize() == 0)
				continue;

			if (srgsToMerge[setNumber].GetSize() == 1)
			{
				if (commitedSRGsBySetNumber[setNumber] != srgsToMerge[setNumber][0])
				{
					commitedSRGsBySetNumber[setNumber] = srgsToMerge[setNumber][0];

					VkDescriptorSet descriptorSet = commitedSRGsBySetNumber[setNumber]->GetDescriptorSet();
					vkCmdBindDescriptorSets(commandBuffer, boundPipeline->GetBindPoint(),
						boundPipeline->GetVkPipelineLayout(), setNumber, 1, &descriptorSet, 0, nullptr);
				}
			}
			else // > 1
			{
				auto first = &*srgsToMerge[setNumber].begin();
				auto last = &*(srgsToMerge[setNumber].end() - 1);
				auto mergedSrg = srgManager->FindOrCreateMergedSRG(srgsToMerge[setNumber].GetSize(), srgsToMerge[setNumber].GetData());

				if (commitedSRGsBySetNumber[setNumber] != (Vulkan::ShaderResourceGroup*)mergedSrg)
				{
					commitedSRGsBySetNumber[setNumber] = mergedSrg;

					VkDescriptorSet descriptorSet = mergedSrg->GetDescriptorSet();
					vkCmdBindDescriptorSets(commandBuffer, boundPipeline->GetBindPoint(),
						boundPipeline->GetVkPipelineLayout(), setNumber, 1, &descriptorSet, 0, nullptr);
				}
			}
		}
	}

	void CommandList::BindPipelineState(RHI::PipelineState* rhiPipelineState)
	{
		if (rhiPipelineState == nullptr)
			return;

		RHI::PipelineStateType pipelineType = rhiPipelineState->GetPipelineType();
		Vulkan::PipelineState* pipelineState = (Vulkan::PipelineState*)rhiPipelineState;

		// No need to bind the same pipeline again
		if (boundPipeline == pipelineState->GetPipeline())
			return;
		
		boundPipeline = pipelineState->GetPipeline();
		
		if (pipelineType == RHI::PipelineStateType::Graphics)
		{
			Vulkan::GraphicsPipeline* gfxPipeline = (Vulkan::GraphicsPipeline*)boundPipeline;
			VkPipeline vkPipeline = gfxPipeline->FindOrCompile(currentPass, currentSubpass);

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
		}
		else if (pipelineType == RHI::PipelineStateType::Compute)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, boundPipeline->GetPipeline());
		}
	}

	void CommandList::BindVertexBuffers(u32 firstInputSlot, u32 count, const RHI::VertexBufferView* bufferViews)
	{
		count = Math::Min(count, RHI::Limits::Pipeline::MaxVertexInputSlotCount);

		FixedArray<VkBuffer, RHI::Limits::Pipeline::MaxVertexInputSlotCount> buffers{};
		FixedArray<VkDeviceSize, RHI::Limits::Pipeline::MaxVertexInputSlotCount> offsets{};

		for (int i = 0; i < count; i++)
		{
			Vulkan::Buffer* buffer = (Vulkan::Buffer*)bufferViews[i].GetBuffer();
			buffers.Add(buffer->GetBuffer());
			offsets.Add((VkDeviceSize)bufferViews[i].GetByteOffset());
		}

		vkCmdBindVertexBuffers(commandBuffer, firstInputSlot, count, buffers.GetData(), offsets.GetData());
	}

	void CommandList::BindIndexBuffer(const RHI::IndexBufferView& bufferView)
	{
		Vulkan::Buffer* buffer = (Vulkan::Buffer*)bufferView.GetBuffer();
		VkIndexType indexType;

		switch (bufferView.GetIndexFormat())
		{
		case RHI::IndexFormat::Uint16:
			indexType = VK_INDEX_TYPE_UINT16;
			break;
		case RHI::IndexFormat::Uint32:
			indexType = VK_INDEX_TYPE_UINT32;
			break;
		}

		vkCmdBindIndexBuffer(commandBuffer, buffer->GetBuffer(), bufferView.GetByteOffset(), indexType);
	}

	void CommandList::DrawIndexed(const DrawIndexedArguments& args)
	{
		vkCmdDrawIndexed(commandBuffer, args.indexCount, args.instanceCount, args.firstIndex, args.vertexOffset, args.firstInstance);
	}

	void CommandList::Begin()
	{
		VkCommandBufferBeginInfo cmdBeginInfo{};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		//cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &cmdBeginInfo);

		boundPipeline = nullptr;
		
		for (int i = 0; i < boundSRGs.GetSize(); i++)
		{
			boundSRGs[i] = nullptr;
		}

		for (int i = 0; i < commitedSRGsBySetNumber.GetSize(); i++)
		{
			commitedSRGsBySetNumber[i] = nullptr;
		}
	}

	void CommandList::End()
	{
		vkEndCommandBuffer(commandBuffer);
	}

} // namespace CE::Vulkan
