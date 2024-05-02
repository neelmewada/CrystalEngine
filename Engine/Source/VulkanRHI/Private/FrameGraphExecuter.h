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

		void WaitUntilIdle() override;

		u32 BeginExecution(const FrameGraphExecuteRequest& executeRequest) override;

		void EndExecution(const FrameGraphExecuteRequest& executeRequest) override;

	private:

		bool ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope, HashSet<ScopeId>& executedScopes, 
			HashSet<Vulkan::SwapChain*>& usedSwapChains);

		VulkanDevice* device = nullptr;
		FrameGraphCompiler* compiler = nullptr;

		u32 currentSubmissionIndex = 0;
		u32 currentImageIndex = 0;
		Array<u32> currentImageIndices{};

		u64 totalFramesSubmitted = 0;
	};
    
} // namespace CE::Vulkan
