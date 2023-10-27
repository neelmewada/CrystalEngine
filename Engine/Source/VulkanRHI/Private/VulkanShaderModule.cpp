
#include "VulkanRHIPrivate.h"
#include "VulkanShaderModule.h"

namespace CE
{
	VulkanShaderModule::VulkanShaderModule(VulkanDevice* device, const RHI::ShaderModuleDesc& desc)
		: device(device), stage(desc.stage), name(desc.name)
	{
		VkShaderModuleCreateInfo shaderCI{};
		shaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderCI.pCode = (const uint32_t*)desc.byteCode;
		shaderCI.codeSize = desc.byteSize;
		
		VkResult result = vkCreateShaderModule(device->GetHandle(), &shaderCI, nullptr, &shaderModule);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Vulkan shader module. Error code {}", (int)result);
		}
	}

	VulkanShaderModule::~VulkanShaderModule()
	{
		if (shaderModule != nullptr)
		{
			vkDestroyShaderModule(device->GetHandle(), shaderModule, nullptr);
			shaderModule = nullptr;
		}
	}

} // namespace CE


