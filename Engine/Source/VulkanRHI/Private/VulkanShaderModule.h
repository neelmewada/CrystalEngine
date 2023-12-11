#pragma once

namespace CE
{
    
	class VulkanShaderModule : public RHI::ShaderModule
	{
	public:

		VulkanShaderModule(VulkanDevice* device, const RHI::ShaderModuleDesc& desc, const ShaderReflection& shaderReflection);
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

		inline VkShaderModule GetHandle() const { return shaderModule; }

	private:

		Name name = "";
		VulkanDevice* device = nullptr;
		VkShaderModule shaderModule = nullptr;
		RHI::ShaderStage stage = RHI::ShaderStage::None;

		HashMap<Name, int> variableBindingMap{};

	};

} // namespace CE
