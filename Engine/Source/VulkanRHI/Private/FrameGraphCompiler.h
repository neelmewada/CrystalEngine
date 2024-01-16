#pragma once

namespace CE::Vulkan
{
	class Scope;
	class CommandList;
    
	class VULKANRHI_API FrameGraphCompiler final : public RHI::FrameGraphCompiler
	{
	public:

		FrameGraphCompiler(VulkanDevice* device);
		virtual ~FrameGraphCompiler();

		void CompileScopesInternal(const FrameGraphCompileRequest& compileRequest) override;
		
		void CompileInternal(const FrameGraphCompileRequest& compileRequest) override;

	private:
        
		void DestroySyncObjects();
		void DestroyCommandLists();

		void CompileCrossQueueDependencies(const FrameGraphCompileRequest& compileRequest, 
			Vulkan::Scope* current = nullptr);

		void CompileBarriers(const FrameGraphCompileRequest& compileRequest);

		VulkanDevice* device = nullptr;

		FixedArray<VkSemaphore, RHI::Limits::Pipeline::MaxSwapChainImageCount> imageAcquiredSemaphores{};
		FixedArray<VkFence, RHI::Limits::Pipeline::MaxSwapChainImageCount> imageAcquiredFences{};

		FixedArray<VkSemaphore, RHI::Limits::Pipeline::MaxSwapChainImageCount> graphExecutedSemaphores{};
		FixedArray<VkFence, RHI::Limits::Pipeline::MaxSwapChainImageCount> graphFinishedFences{};

		FixedArray<Array<Vulkan::CommandList*>, RHI::Limits::Pipeline::MaxSwapChainImageCount> commandListsByImageIndex{};

		u32 imageCount = 0;
		u32 numFramesInFlight = 0;

		friend class FrameGraphExecuter;
	};

} // namespace CE::Vulkan
