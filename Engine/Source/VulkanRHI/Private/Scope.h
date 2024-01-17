#pragma once

namespace CE::Vulkan
{

	class VULKANRHI_API Scope : public RHI::Scope
	{
	public:
		using Super = RHI::Scope;
		using Self = Scope;

		struct Barrier
		{
			VkPipelineStageFlags srcStageMask{};
			VkPipelineStageFlags dstStageMask{};
			bool supportsRegionalDependency = false;

			List<VkMemoryBarrier> memoryBarriers{};
			List<VkBufferMemoryBarrier> bufferBarriers{};
			List<VkImageMemoryBarrier> imageBarriers{};
		};

		Scope(VulkanDevice* device, const RHI::ScopeDescriptor& desc);
		virtual ~Scope();

		virtual bool CompileInternal(const FrameGraphCompileRequest& compileRequest) override;

	private:

		void DestroySyncObjects();
		
		FixedArray<VkSemaphore, RHI::Limits::Pipeline::MaxSwapChainImageCount> renderFinishedSemaphores{};
		FixedArray<VkFence, RHI::Limits::Pipeline::MaxSwapChainImageCount> renderFinishedFences{};

		FixedArray<List<VkSemaphore>, RHI::Limits::Pipeline::MaxSwapChainImageCount> waitSemaphores{};
		List<VkPipelineStageFlags> waitSemaphoreStageFlags{};

		Array<Barrier> barriers{};

		VulkanDevice* device = nullptr;
        CommandQueue* queue = nullptr;
		RenderPass* renderPass = nullptr;
        
		friend class FrameGraphCompiler;
		friend class FrameGraphExecuter;
		friend class RenderPass;
	};
    
} // namespace CE::Vulkan
