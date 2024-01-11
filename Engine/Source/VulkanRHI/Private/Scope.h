#pragma once

namespace CE::Vulkan
{

	class VULKANRHI_API Scope : public RHI::Scope
	{
	public:
		using Super = RHI::Scope;
		using Self = Scope;

		Scope(VulkanDevice* device, const RHI::ScopeDescriptor& desc);
		virtual ~Scope();

		virtual bool CompileInternal(const FrameGraphCompileRequest& compileRequest) override;

	private:

		FixedArray<VkSemaphore, RHI::Limits::Pipeline::MaxSimultaneousFramesInFlight> imageAquiredSemaphores{};
        
		VulkanDevice* device = nullptr;
        CommandQueue* queue = nullptr;
        
        friend class FrameGraphCompiler;
	};
    
} // namespace CE::Vulkan
