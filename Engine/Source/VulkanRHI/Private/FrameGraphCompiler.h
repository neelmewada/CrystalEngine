#pragma once

namespace CE::Vulkan
{
    
	class VULKANRHI_API FrameGraphCompiler final : public RHI::FrameGraphCompiler
	{
	public:
		FrameGraphCompiler(VulkanDevice* device);
		virtual ~FrameGraphCompiler();

		void CompileCrossQueueScopes(const FrameGraphCompilerRequest& compileRequest) override;
		
		void CompileInternal(const FrameGraphCompilerRequest& compileRequest) override;

	private:

		VulkanDevice* device = nullptr;
	};

} // namespace CE::Vulkan
