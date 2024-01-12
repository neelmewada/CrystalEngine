#pragma once

namespace CE::Vulkan
{
	class Scope;
    
	class VULKANRHI_API FrameGraphCompiler final : public RHI::FrameGraphCompiler
	{
	public:
		FrameGraphCompiler(VulkanDevice* device);
		virtual ~FrameGraphCompiler();

		void CompileCrossQueueScopes(const FrameGraphCompileRequest& compileRequest) override;
		
		void CompileInternal(const FrameGraphCompileRequest& compileRequest) override;

	private:
        
		void CompileProducers(const FrameGraphCompileRequest& compileRequest, 
			const Array<RHI::Scope*>& producers, Vulkan::Scope* current = nullptr);

		VulkanDevice* device = nullptr;
	};

} // namespace CE::Vulkan
