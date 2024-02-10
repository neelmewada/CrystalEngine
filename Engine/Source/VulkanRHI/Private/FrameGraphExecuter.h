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

		void ExecuteScopesRecursively(Vulkan::Scope* scope);

		bool ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope, HashSet<ScopeID>& executedScopes);

		VulkanDevice* device = nullptr;
		FrameGraphCompiler* compiler = nullptr;

		u32 currentSubmissionIndex = 0;
		u32 currentImageIndex = 0;

		u64 totalFramesSubmitted = 0;
	};
    
} // namespace CE::Vulkan
