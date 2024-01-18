#pragma once

namespace CE::RHI
{

	/// @brief FrameAttachment is the class used to store a single re-usable attachment throughout a frame.
	/// FrameAttachment is converted to ScopeAttachment when attached to a specific scope (aka pass).
	class CORERHI_API FrameAttachment
	{
	public:
        
		virtual ~FrameAttachment();

		void SetResource(RHIResource* resource);

		void SetResource(int imageIndex, RHIResource* resource);

		RHIResource* GetResource(int imageIndex);
		RHIResource* GetResource();

		virtual bool IsBufferAttachment() const = 0;
		virtual bool IsImageAttachment() const = 0;
		virtual bool IsSwapChainAttachment() const { return false; }

		inline AttachmentID GetId() const { return attachmentId; }

		inline AttachmentLifetimeType GetLifetimeType() const { return lifetime; }
        
    protected:
        
        FrameAttachment(const AttachmentID& attachmentId, AttachmentLifetimeType lifetime);

	protected:
        
        AttachmentID attachmentId{};
        AttachmentLifetimeType lifetime{};
		//! Either a buffer or texture
		StaticArray<RHIResource*, RHI::Limits::Pipeline::MaxSwapChainImageCount> resources{};
		//RHIResource* resource = nullptr;
	};
    
} // namespace CE::RHI
