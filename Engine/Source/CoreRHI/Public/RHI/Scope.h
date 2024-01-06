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

		void AddScopeAttachment(ScopeAttachment* attachment);

		bool ScopeAttachmentExists(const Name& id);

		ScopeAttachment* FindScopeAttachment(const Name& id);

		template<typename ScopeAttachmentType, typename DescriptorType = ScopeAttachmentType::DescriptorType>
		ScopeAttachmentType* EmplaceScopeAttachment(FrameAttachment* attachment,
			ScopeAttachmentUsage usage,
			ScopeAttachmentAccess access,
			const DescriptorType& descriptor);

    private:

		//! @brief The frame graph that owns this scope.
		FrameGraph* frameGraph = nullptr;

		RHI::HardwareQueueClass queueClass{};

		Name id{};
        
		//! @brief List of all scope attachments owned by this scope.
		Array<ScopeAttachment*> attachments{};

		Array<ImageScopeAttachment> imageAttachments{};

		Array<BufferScopeAttachment> bufferAttachments{};


		friend class FrameGraph;
		friend class FrameGraphBuilder;
    };

	template<typename ScopeAttachmentType, typename DescriptorType>
	inline ScopeAttachmentType* Scope::EmplaceScopeAttachment(FrameAttachment* attachment, 
		ScopeAttachmentUsage usage, 
		ScopeAttachmentAccess access, 
		const DescriptorType& descriptor)
	{
		ScopeAttachmentType* scopeAttachment = new ScopeAttachmentType(this, attachment, usage, access, descriptor);
		attachments.Add(scopeAttachment);
		return scopeAttachment;
	}

} // namespace CE::RHI
