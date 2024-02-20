
#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	ShaderModule::ShaderModule(VulkanDevice* device, const RHI::ShaderModuleDescriptor& desc)
		: device(device)
	{
		this->stage = desc.stage;
		this->isValid = false;

		if (desc.byteSize == 0 || desc.byteCode == nullptr)
			return;

		VkShaderModuleCreateInfo shaderCI{};
		shaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderCI.pCode = (const uint32_t*)desc.byteCode;
		shaderCI.codeSize = desc.byteSize;

		hash = CalculateHash(desc.byteCode, desc.byteSize);
		
		VkResult result = vkCreateShaderModule(device->GetHandle(), &shaderCI, nullptr, &shaderModule);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan shader module. Error code {}", (int)result);
			return;
		}

		isValid = true;
	}

	ShaderModule::~ShaderModule()
	{
		isValid = false;

		if (shaderModule != nullptr)
		{
			vkDestroyShaderModule(device->GetHandle(), shaderModule, nullptr);
			shaderModule = nullptr;
		}
	}

} // namespace CE


