#pragma once

namespace CE::Vulkan
{
    
	class ShaderModule : public RHI::ShaderModule
	{
	public:

		ShaderModule(VulkanDevice* device, const RHI::ShaderModuleDescriptor& desc);
		~ShaderModule();

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
