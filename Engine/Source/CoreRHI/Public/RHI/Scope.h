#pragma once

namespace CE::RHI
{
	class FrameGraph;

	struct ScopeDescriptor
	{
		Name id{};
		RHI::HardwareQueueClass queueClass{};
	};

    class CORERHI_API Scope
    {
    public:
        virtual ~Scope();
        
	protected:

		Scope(const ScopeDescriptor& desc);

    private:

		//! @brief The frame graph that owns this scope.
		FrameGraph* frameGraph = nullptr;

		RHI::HardwareQueueClass queueClass{};

		Name id{};
        
		Array<ScopeAttachment*> attachments{};

		Array<ImageScopeAttachment> imageAttachments{};

		Array<BufferScopeAttachment> bufferAttachments{};


		friend class FrameGraph;
		friend class FrameGraphBuilder;
    };

} // namespace CE::RHI
