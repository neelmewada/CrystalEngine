#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{

	ShaderResourceManager::ShaderResourceManager(VulkanDevice* device)
		: device(device)
	{
		const auto& limits = device->GetDeviceLimits();
		maxBoundDescriptorSets =  limits.maxBoundDescriptorSets;
        
        if (maxBoundDescriptorSets == 4)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 0 });
            srgSlots.Add({ RHI::SRGType::PerPass, 0 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 0 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 1 });
            srgSlots.Add({ RHI::SRGType::PerObject, 2 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 3 });
        }
        else if (maxBoundDescriptorSets == 5)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 0 });
            srgSlots.Add({ RHI::SRGType::PerPass, 1 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 1 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 2 });
            srgSlots.Add({ RHI::SRGType::PerObject, 3 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 4 });
        }
        else if (maxBoundDescriptorSets == 6)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 1 });
            srgSlots.Add({ RHI::SRGType::PerPass, 2 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 2 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 3 });
            srgSlots.Add({ RHI::SRGType::PerObject, 4 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 5 });
        }
        else if (maxBoundDescriptorSets >= 7)
        {
            srgSlots.Add({ RHI::SRGType::PerScene, 0 });
            srgSlots.Add({ RHI::SRGType::PerView, 1 });
            srgSlots.Add({ RHI::SRGType::PerPass, 2 });
            srgSlots.Add({ RHI::SRGType::PerSubPass, 3 });
            srgSlots.Add({ RHI::SRGType::PerMaterial, 4 });
            srgSlots.Add({ RHI::SRGType::PerObject, 5 });
            srgSlots.Add({ RHI::SRGType::PerDraw, 6 });
        }
        
        for (const SRGSlot& slot : srgSlots)
        {
			srgTypeToDescriptorSet[slot.srgType] = slot;
			setNumberToSrgs[slot.set].Add(slot);
        }
	}

	ShaderResourceManager::~ShaderResourceManager()
	{
		vkDeviceWaitIdle(device->GetHandle());

		for (auto [hash, srg] : mergedSRGsByHash)
		{
			delete srg;
		}
		mergedSRGsByHash.Clear();

		for (auto set : queuedDestroySets)
		{
			delete set;
		}
		queuedDestroySets.Clear();
	}

	bool ShaderResourceManager::IsSharedDescriptorSet(RHI::SRGType srgType)
	{
		if (!srgTypeToDescriptorSet.KeyExists(srgType))
			return false;

		int set = srgTypeToDescriptorSet[srgType].set;
		return IsSharedDescriptorSet(set);
	}

	int ShaderResourceManager::GetDescriptorSetNumber(RHI::SRGType srgType)
	{
		if (!srgTypeToDescriptorSet.KeyExists(srgType))
			return false;

		return srgTypeToDescriptorSet[srgType].set;
	}

	bool ShaderResourceManager::ShouldCreateDescriptorSetForSRG(RHI::SRGType srgType)
	{
		if (!srgTypeToDescriptorSet.KeyExists(srgType))
			return false;

		int set = srgTypeToDescriptorSet[srgType].set;
		const auto& array = setNumberToSrgs[set];
		if (array.IsEmpty())
			return false;

		return array.GetLast().srgType == srgType;
	}

	VkDescriptorType ShaderResourceManager::GetDescriptorType(ShaderResourceType shaderResourceType, bool usesDynamicOffset)
	{
		switch (shaderResourceType)
		{
		case ShaderResourceType::ConstantBuffer:
			return usesDynamicOffset ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case ShaderResourceType::StructuredBuffer:
		case ShaderResourceType::RWStructuredBuffer:
			return usesDynamicOffset ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case ShaderResourceType::Texture1D:
		case ShaderResourceType::Texture2D:
		case ShaderResourceType::Texture3D:
		case ShaderResourceType::TextureCube:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case ShaderResourceType::RWTexture2D:
		case ShaderResourceType::RWTexture3D:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case ShaderResourceType::SubpassInput:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case ShaderResourceType::SamplerState:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		}

		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

	MergedShaderResourceGroup* ShaderResourceManager::FindOrCreateMergedSRG(u32 srgCount, ShaderResourceGroup** srgs)
	{
		SIZE_T mergedSRGHash = 0;
		FixedArray<Vulkan::ShaderResourceGroup*, RHI::Limits::Pipeline::MaxShaderResourceGroupCount> srgArray{};
		
		for (int i = 0; i < srgCount; i++)
		{
			srgArray.Add(srgs[i]);
		}
        
		std::sort(srgArray.begin(), srgArray.end(),
            [](Vulkan::ShaderResourceGroup* a, Vulkan::ShaderResourceGroup* b) -> bool
			{
				return (int)a->GetSRGType() <= (int)b->GetSRGType();
			});

		mergedSRGHash = (SIZE_T)srgArray[0];
		for (int i = 1; i < srgArray.GetSize(); i++)
		{
			mergedSRGHash = GetCombinedHash(mergedSRGHash, (SIZE_T)srgArray[i]);
		}

		if (mergedSRGsByHash.KeyExists(mergedSRGHash) && mergedSRGsByHash[mergedSRGHash] != nullptr)
		{
			return mergedSRGsByHash[mergedSRGHash];
		}

		return CreateMergedSRG(srgArray.GetSize(), srgArray.GetData());
	}

	MergedShaderResourceGroup* ShaderResourceManager::CreateMergedSRG(u32 srgCount, ShaderResourceGroup** srgs)
	{
		if (srgCount)
			return nullptr;

		MergedShaderResourceGroup* mergedSRG = new MergedShaderResourceGroup(device, srgCount, srgs);
		if (mergedSRG->GetMergedHash() == 0)
		{
			delete mergedSRG;
			return nullptr;
		}
		
		mergedSRGsByHash[mergedSRG->GetMergedHash()] = mergedSRG;

		return mergedSRG;
	}

	void ShaderResourceManager::RemoveMergedSRG(MergedShaderResourceGroup* mergedSRG)
	{
		if (!mergedSRG)
			return;

		mergedSRGsByHash.Remove(mergedSRG->GetMergedHash());

		for (auto sourceSRG : mergedSRG->combinedSRGs)
		{
			mergedSRGsBySourceSRG[sourceSRG].Remove(mergedSRG);
		}
	}

	void ShaderResourceManager::OnSRGDestroyed(ShaderResourceGroup* srg)
	{
		if (!mergedSRGsBySourceSRG.KeyExists(srg))
			return;

		Array<MergedShaderResourceGroup*> srgsToDestroy{};
		srgsToDestroy.Resize(mergedSRGsBySourceSRG[srg].GetSize());

		int i = 0;
		
		// Destroy all Merged SRGs created from the given source 'srg'
		for (MergedShaderResourceGroup* mergedSRG : mergedSRGsBySourceSRG[srg])
		{
			// Get all source SRGs used to create 'mergeSRG'
			for (ShaderResourceGroup* sourceSRG : mergedSRG->combinedSRGs)
			{
				// Remove reference to 'mergeSRG' from all the source SRGs used to create it.
				mergedSRGsBySourceSRG[sourceSRG].Remove(mergedSRG);
			}

			mergedSRGsByHash.Remove(mergedSRG->GetMergedHash());
			srgsToDestroy[i++] = mergedSRG;
		}

		mergedSRGsBySourceSRG[srg].Clear();
		mergedSRGsBySourceSRG.Remove(srg);
		
		for (auto destroySrg : srgsToDestroy)
		{
			destroySrg->combinedSRGs.Clear();
			delete destroySrg;
		}
	}

	void ShaderResourceManager::DestroyQueuedSRG()
	{
		if (destroyQueue.GetSize() >= 32)
		{
			vkDeviceWaitIdle(device->GetHandle());

			// Do NOT use a for loop here, 'delete srg' can append to destroyQueue recursively!
			while (destroyQueue.NonEmpty())
			{
				auto srg = destroyQueue[0];
				destroyQueue.RemoveAt(0);
				delete srg;
			}
			destroyQueue.Clear();
		}

		for (int i = queuedDestroySets.GetSize() - 1; i >= 0; i--)
		{
			if (queuedDestroySets[i]->usageCount <= 0)
			{
				delete queuedDestroySets[i];
				queuedDestroySets.RemoveAt(i);
			}
		}
	}

	void ShaderResourceManager::QueueDestroy(DescriptorSet* descriptorSet)
	{
		if (descriptorSet)
		{
			queuedDestroySets.Add(descriptorSet);
		}
	}

    ShaderResourceGroup::ShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupLayout& srgLayout)
		: device(device)
    {
		this->srgType = srgLayout.srgType;
		this->srgLayout = srgLayout;
		this->srgManager = device->GetShaderResourceManager();
		pool = device->GetDescriptorPool();
		setNumber = srgManager->GetDescriptorSetNumber(srgType);

		setLayoutBindings.Clear();
		bindingSlotsByVariableName.Clear();
		variableBindingsByName.Clear();
		variableBindingsBySlot.Clear();

		for (const RHI::SRGVariableDescriptor& variable : srgLayout.variables)
		{
			setLayoutBindings.Add({});
			VkDescriptorSetLayoutBinding& entry = setLayoutBindings.GetLast();
			entry.binding = variable.bindingSlot;
			entry.descriptorCount = variable.arrayCount;
			entry.descriptorType = srgManager->GetDescriptorType(variable.type, variable.usesDynamicOffset);
			entry.stageFlags = 0;

			if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Vertex))
			{
				entry.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			}
			if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Fragment))
			{
				entry.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			if (EnumHasFlag(variable.shaderStages, RHI::ShaderStage::Tessellation))
			{
				entry.stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			}

			bindingSlotsByVariableName[variable.name] = variable.bindingSlot;
			variableBindingsByName[variable.name] = entry;
			variableBindingsBySlot[variable.bindingSlot] = entry;
		}
    }

    ShaderResourceGroup::~ShaderResourceGroup()
	{
		//vkDeviceWaitIdle(device->GetHandle());

		srgManager->OnSRGDestroyed(this);
		QueueDestroy();
	}

	void ShaderResourceGroup::FlushBindings()
	{
		if (!isCompiled)
		{
			Compile();
		}
		else if (needsRecompile) // Free current DescriptorSet and create new one
		{
			QueueDestroy();
			Compile();
		}
	}

	ShaderResourceGroup::ShaderResourceGroup(VulkanDevice* device)
		: device(device)
	{
		this->srgManager = device->GetShaderResourceManager();
		pool = device->GetDescriptorPool();
	}

	bool ShaderResourceGroup::Bind(Name name, RHI::BufferView bufferView)
	{
		if (!bufferView.GetBuffer())
			return false;
		if (!bindingSlotsByVariableName.KeyExists(name))
			return false;

		int bindingSlot = bindingSlotsByVariableName[name];

		// Something was already bound at this position, cannot re-bind here.
		if (bufferInfosBoundBySlot.KeyExists(bindingSlot))
			return false;
		if (imageInfosBoundBySlot.KeyExists(bindingSlot))
			return false;

		auto size = bufferView.GetByteCount();

		VkDescriptorBufferInfo bufferWrite{};
		bufferWrite.buffer = (VkBuffer)bufferView.GetBuffer()->GetHandle();
		bufferWrite.offset = bufferView.GetByteOffset();
		bufferWrite.range = size > 0 ? size : bufferView.GetBuffer()->GetBufferSize();
		
		bufferInfosBoundBySlot[bindingSlot].Clear();
		bufferInfosBoundBySlot[bindingSlot].Add(bufferWrite);
		
		needsRecompile = true;
		return true;
	}

	bool ShaderResourceGroup::Bind(Name name, u32 count, RHI::BufferView* bufferViews)
	{
		if (!bindingSlotsByVariableName.KeyExists(name))
			return false;

		int bindingSlot = bindingSlotsByVariableName[name];

		// Something was already bound at this position, cannot re-bind here.
		if (bufferInfosBoundBySlot.KeyExists(bindingSlot))
			return false;
		if (imageInfosBoundBySlot.KeyExists(bindingSlot))
			return false;

		bufferInfosBoundBySlot[bindingSlot].Clear();

		for (int i = 0; i < count; i++)
		{
			auto size = bufferViews[i].GetByteCount();

			VkDescriptorBufferInfo bufferWrite{};
			bufferWrite.buffer = (VkBuffer)bufferViews[i].GetBuffer()->GetHandle();
			bufferWrite.offset = bufferViews[i].GetByteOffset();
			bufferWrite.range = size > 0 ? size : bufferViews[i].GetBuffer()->GetBufferSize();

			bufferInfosBoundBySlot[bindingSlot].Add(bufferWrite);
		}

		needsRecompile = true;
		return true;
	}

	bool ShaderResourceGroup::Bind(Name name, RHI::Texture* rhiTexture)
	{
		if (!rhiTexture)
			return false;
		if (!bindingSlotsByVariableName.KeyExists(name))
			return false;

		int bindingSlot = bindingSlotsByVariableName[name];
		Vulkan::Texture* texture = (Vulkan::Texture*)rhiTexture;

		// Something was already bound at this position, cannot re-bind here.
		if (bufferInfosBoundBySlot.KeyExists(bindingSlot))
			return false;
		if (imageInfosBoundBySlot.KeyExists(bindingSlot))
			return false;

		VkDescriptorImageInfo imageWrite{};
		imageWrite.imageLayout = texture->GetVkImageLayout();
		imageWrite.imageView = texture->GetImageView();
		imageWrite.sampler = nullptr;

		imageInfosBoundBySlot[bindingSlot].Clear();
		imageInfosBoundBySlot[bindingSlot].Add(imageWrite);

		needsRecompile = true;
		return true;
	}

	bool ShaderResourceGroup::Bind(Name name, u32 count, RHI::Texture** textures)
	{
		if (!bindingSlotsByVariableName.KeyExists(name))
			return false;

		int bindingSlot = bindingSlotsByVariableName[name];
		// Something was already bound at this position, cannot re-bind here.
		if (bufferInfosBoundBySlot.KeyExists(bindingSlot))
			return false;
		if (imageInfosBoundBySlot.KeyExists(bindingSlot))
			return false;

		imageInfosBoundBySlot[bindingSlot].Clear();

		for (int i = 0; i < count; i++)
		{
			Vulkan::Texture* texture = (Vulkan::Texture*)textures[i];

			VkDescriptorImageInfo imageWrite{};
			imageWrite.imageLayout = texture->GetVkImageLayout();
			imageWrite.imageView = texture->GetImageView();
			imageWrite.sampler = nullptr;

			imageInfosBoundBySlot[bindingSlot].Add(imageWrite);
		}

		needsRecompile = true;
		return true;
	}

	bool ShaderResourceGroup::Bind(Name name, RHI::Sampler* rhiSampler)
	{
		if (!rhiSampler)
			return false;
		if (!bindingSlotsByVariableName.KeyExists(name))
			return false;

		int bindingSlot = bindingSlotsByVariableName[name];
		Vulkan::Sampler* sampler = (Vulkan::Sampler*)rhiSampler;

		// Something was already bound at this position, cannot re-bind here.
		if (bufferInfosBoundBySlot.KeyExists(bindingSlot))
			return false;
		if (imageInfosBoundBySlot.KeyExists(bindingSlot))
			return false;

		VkDescriptorImageInfo imageWrite{};
		imageWrite.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageWrite.imageView = VK_NULL_HANDLE;
		imageWrite.sampler = sampler->GetVkHandle();

		imageInfosBoundBySlot[bindingSlot].Clear();
		imageInfosBoundBySlot[bindingSlot].Add(imageWrite);

		needsRecompile = true;
		return true;
	}

	bool ShaderResourceGroup::Bind(Name name, u32 count, RHI::Sampler** samplers)
	{
		if (!bindingSlotsByVariableName.KeyExists(name))
			return false;

		int bindingSlot = bindingSlotsByVariableName[name];
		// Something was already bound at this position, cannot re-bind here.
		if (bufferInfosBoundBySlot.KeyExists(bindingSlot))
			return false;
		if (imageInfosBoundBySlot.KeyExists(bindingSlot))
			return false;

		imageInfosBoundBySlot[bindingSlot].Clear();

		for (int i = 0; i < count; i++)
		{
			Vulkan::Sampler* sampler = (Vulkan::Sampler*)samplers[i];
			if (sampler == nullptr)
				continue;

			VkDescriptorImageInfo imageWrite{};
			imageWrite.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageWrite.imageView = VK_NULL_HANDLE;
			imageWrite.sampler = sampler->GetVkHandle();
			
			imageInfosBoundBySlot[bindingSlot].Add(imageWrite);
		}

		needsRecompile = true;
		return true;
	}

	void ShaderResourceGroup::Compile()
	{
		if (isCompiled)
			return;

		descriptorSet = new DescriptorSet(device, srgLayout);

		UpdateBindings();

		isCompiled = true;
		needsRecompile = false;
	}

	void ShaderResourceGroup::QueueDestroy()
	{
		if (descriptorSet)
		{
			srgManager->QueueDestroy(descriptorSet);
			descriptorSet = nullptr;
		}

		isCommitted = isCompiled = false;
	}

	void ShaderResourceGroup::UpdateBindings()
	{
		if (descriptorSet == nullptr)
			return;

		Array<VkWriteDescriptorSet> writes{};
		writes.Resize(bufferInfosBoundBySlot.GetSize() + imageInfosBoundBySlot.GetSize());
		int idx = 0;

		for (const auto& [slot, bufferWrites] : bufferInfosBoundBySlot)
		{
			if (!variableBindingsBySlot.KeyExists(slot))
				continue;

			VkDescriptorSetLayoutBinding& variable = variableBindingsBySlot[slot];

			VkWriteDescriptorSet& write = writes[idx++];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorCount = variable.descriptorCount;
			write.descriptorType = variable.descriptorType;
			write.dstArrayElement = 0;
			write.dstBinding = variable.binding;
			write.dstSet = descriptorSet->GetHandle();
			write.pBufferInfo = bufferWrites.GetData();
		}

		for (const auto& [slot, imageWrites] : imageInfosBoundBySlot)
		{
			if (!variableBindingsBySlot.KeyExists(slot))
				continue;

			VkDescriptorSetLayoutBinding& variable = variableBindingsBySlot[slot];

			VkWriteDescriptorSet& write = writes[idx++];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorCount = variable.descriptorCount;
			write.descriptorType = variable.descriptorType;
			write.dstArrayElement = 0;
			write.dstBinding = variable.binding;
			write.dstSet = descriptorSet->GetHandle();
			write.pImageInfo = imageWrites.GetData();
		}

		vkUpdateDescriptorSets(device->GetHandle(), writes.GetSize(), writes.GetData(), 0, nullptr);
	}

} // namespace CE

