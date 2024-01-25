#pragma once

namespace CE::Vulkan
{
	class ShaderResourceManager;

	class CommandList : public RHI::CommandList
	{
	public:
		CommandList(VulkanDevice* device, VkCommandBuffer commandBuffer, VkCommandBufferLevel level, u32 queueFamilyIndex, VkCommandPool pool);

		virtual ~CommandList();

		inline u32 GetQueueFamilyIndex() const { return queueFamilyIndex; }

		inline VkCommandPool GetCommandPool() const { return pool; }

		inline VkCommandBuffer GetCommandBuffer() const { return commandBuffer; }

		void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs) override;

	private:
		
		VulkanDevice* device = nullptr;
		ShaderResourceManager* srgManager = nullptr;

		VkCommandBuffer commandBuffer = nullptr;
		VkCommandPool pool = nullptr;
		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		u32 queueFamilyIndex = 0;


		HashMap<int, Vulkan::ShaderResourceGroup*> boundSRGs{};

		friend class FrameGraphCompiler;
		friend class FrameGraphExecuter;
	};
    
} // namespace CE::Vulkan
