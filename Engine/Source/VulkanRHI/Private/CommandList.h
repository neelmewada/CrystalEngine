#pragma once

namespace CE::Vulkan
{
	class Scope;
	class ShaderResourceManager;

	class CommandList : public RHI::CommandList
	{
	public:
		CommandList(VulkanDevice* device, VkCommandBuffer commandBuffer, RHI::CommandListType type, u32 queueFamilyIndex, VkCommandPool pool);

		virtual ~CommandList();

		inline u32 GetQueueFamilyIndex() const { return queueFamilyIndex; }

		inline VkCommandPool GetCommandPool() const { return pool; }

		inline VkCommandBuffer GetCommandBuffer() const { return commandBuffer; }

		void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs) override;

		void ClearShaderResourceGroups() override;

		void CommitShaderResources() override;

		void BindPipelineState(RHI::PipelineState* pipelineState) override;

		void BindVertexBuffers(u32 firstInputSlot, u32 count, const RHI::VertexBufferView* bufferViews) override;

		void BindIndexBuffer(const RHI::IndexBufferView& bufferViews) override;

		void DrawIndexed(const DrawIndexedArguments& args) override;

		void Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) override;

		void ResourceBarrier(u32 count, RHI::ResourceBarrierDescriptor* barriers) override;

		void CopyTextureRegion(const BufferToTextureCopy& region) override;

		void Begin() override;
		void End() override;

	private:
		
		VulkanDevice* device = nullptr;
		ShaderResourceManager* srgManager = nullptr;

		VkCommandBuffer commandBuffer = nullptr;
		VkCommandPool pool = nullptr;
		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		u32 queueFamilyIndex = 0;
		VkQueueFlags curQueueFlags{};

		RenderPass* currentPass = nullptr;
		u32 currentSubpass = 0;
		Vulkan::Pipeline* boundPipeline = nullptr;

		StaticArray<Vulkan::ShaderResourceGroup*, RHI::Limits::Pipeline::MaxShaderResourceGroupCount> boundSRGs{};
		StaticArray<Vulkan::DescriptorSet*, RHI::Limits::Pipeline::MaxShaderResourceGroupCount> commitedSRGsBySetNumber{};

		friend class FrameGraphCompiler;
		friend class FrameGraphExecuter;
	};
    
} // namespace CE::Vulkan
