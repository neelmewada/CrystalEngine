#pragma once

namespace CE::Vulkan
{

	class VULKANRHI_API Scope : public RHI::Scope
	{
	public:
		using Super = RHI::Scope;

		Scope(const RHI::ScopeDescriptor& desc);
		virtual ~Scope();

	private:
        
        CommandQueue* queue = nullptr;
        
        friend class FrameGraphCompiler;
	};
    
} // namespace CE::Vulkan
