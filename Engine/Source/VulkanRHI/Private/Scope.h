#pragma once

namespace CE::Vulkan
{

	class VULKANRHI_API Scope : public RHI::Scope
	{
	public:
		using Super = RHI::Scope;
		using Self = Scope;

		struct ImageLayoutTransition
		{
			Vulkan::Texture* image{};
			VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
			u32 queueFamilyIndex = 0;
		};

		struct Barrier
		{
			VkPipelineStageFlags srcStageMask{};
			VkPipelineStageFlags dstStageMask{};
			bool supportsRegionalDependency = false;

			List<VkMemoryBarrier> memoryBarriers{};
			List<VkBufferMemoryBarrier> bufferBarriers{};
			List<VkImageMemoryBarrier> imageBarriers{};

			//! Just to keep track of image layout
			List<ImageLayoutTransition> imageLayoutTransitions{};
		};

		Scope(VulkanDevice* device, const RHI::ScopeDescriptor& desc);
		virtual ~Scope();

		virtual bool CompileInternal(const FrameGraphCompileRequest& compileRequest) override;

	private:

		void DestroySyncObjects();
		
		FixedArray<VkSemaphore, RHI::Limits::Pipeline::MaxSwapChainImageCount> renderFinishedSemaphores{};
		FixedArray<VkFence, RHI::Limits::Pipeline::MaxSwapChainImageCount> renderFinishedFences{};

		FixedArray<List<VkSemaphore>, RHI::Limits::Pipeline::MaxSwapChainImageCount> waitSemaphores{};
		FixedArray<FrameBuffer*, RHI::Limits::Pipeline::MaxSwapChainImageCount> frameBuffers{};

		List<VkPipelineStageFlags> waitSemaphoreStageFlags{};

		StaticArray<Array<Barrier>, RHI::Limits::Pipeline::MaxSwapChainImageCount> barriers{};

		VulkanDevice* device = nullptr;
        CommandQueue* queue = nullptr;
		RenderPass* renderPass = nullptr;
        
		friend class FrameGraphCompiler;
		friend class FrameGraphExecuter;
		friend class RenderPass;
		friend class FrameBuffer;
	};
    
} // namespace CE::Vulkan
