#pragma once

namespace CE
{
    
	class VulkanShaderModule : public RHI::ShaderModule
	{
	public:

		VulkanShaderModule(VulkanDevice* device, const RHI::ShaderModuleDesc& desc);
		~VulkanShaderModule();

		Name GetName() override
		{
			return name;
		}

		bool IsValidShader() override
		{
			return shaderModule != nullptr;
		}

		RHI::ShaderStage GetShaderStage() override
		{
			return stage;
		}

	private:

		Name name = "";
		VulkanDevice* device = nullptr;
		VkShaderModule shaderModule = nullptr;
		RHI::ShaderStage stage = RHI::ShaderStage::None;
	};

} // namespace CE
