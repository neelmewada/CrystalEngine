#pragma once

namespace CE::Vulkan
{

	class VULKANRHI_API FrameGraphExecuter final : public RHI::FrameGraphExecuter
	{
	public:
		using Super = RHI::FrameGraphExecuter;
		using Self = Vulkan::FrameGraphExecuter;

		FrameGraphExecuter(VulkanDevice* device);
		~FrameGraphExecuter();

		bool ExecuteInternal(const FrameGraphExecuteRequest& executeRequest) override;

		bool CompileInternal(const FrameGraphCompileRequest& compileRequest) override;

	private:

		void DestroySyncObjects();

		bool ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope);

		VulkanDevice* device = nullptr;

		FixedArray<VkSemaphore, RHI::Limits::Pipeline::MaxFramesInFlight> imageAcquiredSemaphores{};
		FixedArray<VkFence, RHI::Limits::Pipeline::MaxFramesInFlight> imageAcquiredFence{};

		uint32_t currentSubmissionIndex = 0;
	};
    
} // namespace CE::Vulkan
