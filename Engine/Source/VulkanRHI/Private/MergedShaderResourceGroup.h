#pragma once

namespace CE::Vulkan
{
	class VulkanDevice;
    
	class VULKANRHI_API MergedShaderResourceGroup final : public ShaderResourceGroup
	{
	public:

		MergedShaderResourceGroup(VulkanDevice* device, const ArrayView<Vulkan::ShaderResourceGroup*>& shaderResourceGroups);

		~MergedShaderResourceGroup();

		inline SIZE_T GetMergedHash() const
		{
			return mergedHash;
		}

		virtual bool IsMerged() const override final { return true; }

	private:

		Array<ShaderResourceGroup*> combinedSRGs{};
		SIZE_T mergedHash = 0;

		friend class ShaderResourceManager;
	};

} // namespace CE::Vulkan
