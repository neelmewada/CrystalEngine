#pragma once

namespace CE::Vulkan
{
    
	class ShaderModule : public RHI::ShaderModule
	{
	public:

		ShaderModule(VulkanDevice* device, const RHI::ShaderModuleDescriptor& desc);
		~ShaderModule();

		inline VkShaderModule GetHandle() const { return shaderModule; }

	private:

		VulkanDevice* device = nullptr;
		VkShaderModule shaderModule = nullptr;

		HashMap<Name, int> variableBindingMap{};

	};

} // namespace CE
