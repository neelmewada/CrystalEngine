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

	private:


		bool ExecuteScope(const FrameGraphExecuteRequest& executeRequest, Vulkan::Scope* scope);

		VulkanDevice* device = nullptr;

		u32 currentSubmissionIndex = 0;
		u32 currentImageIndex = 0;
	};
    
} // namespace CE::Vulkan
