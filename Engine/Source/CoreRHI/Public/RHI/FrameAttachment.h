#pragma once

namespace CE::RHI
{

	/// @brief FrameAttachment is the class used to store a single re-usable attachment throughout a frame.
	/// FrameAttachment is converted to ScopeAttachment when attached to a specific scope (aka pass).
	class CORERHI_API FrameAttachment
	{
	public:
        
        virtual ~FrameAttachment() = default;

		void SetResource(RHIResource* resource);
        
    protected:
        
        FrameAttachment(const AttachmentID& attachmentId, AttachmentLifetimeType lifetime);

	private:
        
        AttachmentID attachmentId{};
        AttachmentLifetimeType lifetime{};
		//! Either a buffer or texture
		RHIResource* resource = nullptr;
	};
    
} // namespace CE::RHI
