#pragma once

namespace CE::Vulkan
{
	class FrameGraphCompiler;
}

namespace CE::RHI
{
	class FrameGraph;
	struct FrameGraphCompileRequest;

	struct ScopeDescriptor
	{
		ScopeID id{};
		RHI::HardwareQueueClass queueClass{};
	};

    class CORERHI_API Scope
    {
    public:
        virtual ~Scope();
        
	protected:

		Scope(const ScopeDescriptor& desc);

	public:

		inline ScopeID GetId() const { return id; }

		void AddScopeAttachment(ScopeAttachment* attachment);

		bool ScopeAttachmentExists(const Name& id);

		ScopeAttachment* FindScopeAttachment(const Name& id);

		template<typename ScopeAttachmentType, typename DescriptorType = ScopeAttachmentType::DescriptorType> requires TIsBaseClassOf<ScopeAttachment, ScopeAttachmentType>::Value
		ScopeAttachmentType* EmplaceScopeAttachment(FrameAttachment* attachment,
			ScopeAttachmentUsage usage,
			ScopeAttachmentAccess access,
			const DescriptorType& descriptor);

		inline bool PresentsSwapChain() const { return presentsSwapChain; }

		bool Compile(const FrameGraphCompileRequest& compileRequest);

		bool UsesAttachment(FrameAttachment* attachment);

		bool UsesAttachment(AttachmentID attachmentId);

	protected:

		virtual bool CompileInternal(const FrameGraphCompileRequest& compileRequest) {}

		//! @brief The frame graph that owns this scope.
		FrameGraph* frameGraph = nullptr;

		RHI::HardwareQueueClass queueClass{};

		ScopeID id{};

		DrawList drawList{};
        
		//! @brief List of all scope attachments owned by this scope.
		Array<ScopeAttachment*> attachments{};

		Array<ImageScopeAttachment*> imageAttachments{};
		Array<BufferScopeAttachment*> bufferAttachments{};
		Array<ScopeAttachment*> readAttachments{};
		Array<ScopeAttachment*> writeAttachments{};

		bool presentsSwapChain = false;

		friend class FrameGraph;
        friend class FrameGraphCompiler;
		friend class CE::Vulkan::FrameGraphCompiler;
		friend class FrameGraphBuilder;
		friend class FrameScheduler;
    };

	template<typename ScopeAttachmentType, typename DescriptorType> requires TIsBaseClassOf<ScopeAttachment, ScopeAttachmentType>::Value
	inline ScopeAttachmentType* Scope::EmplaceScopeAttachment(FrameAttachment* attachment, 
		ScopeAttachmentUsage usage, 
		ScopeAttachmentAccess access, 
		const DescriptorType& descriptor)
	{
		ScopeAttachmentType* scopeAttachment = new ScopeAttachmentType(this, attachment, usage, access, descriptor);
		AddScopeAttachment(scopeAttachment);
		return scopeAttachment;
	}

} // namespace CE::RHI
