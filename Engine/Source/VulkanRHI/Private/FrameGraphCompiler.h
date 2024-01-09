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
        
        struct QueueID
        {
            int familyIdx = -1;
            int queueIdx = -1;
        };
        
		VulkanDevice* device = nullptr;
	};

} // namespace CE::Vulkan
