#pragma once

namespace CE::Vulkan
{
	class VulkanDevice;
    
	class VULKANRHI_API MergedShaderResourceGroup final : public ShaderResourceGroup
	{
	public:

		MergedShaderResourceGroup(VulkanDevice* device, const Array<RHI::ShaderResourceGroup*>& shaderResourceGroups);

		~MergedShaderResourceGroup();

	private:

		Array<RHI::ShaderResourceGroup*> shaderResourceGroups{};


	};

} // namespace CE::Vulkan
