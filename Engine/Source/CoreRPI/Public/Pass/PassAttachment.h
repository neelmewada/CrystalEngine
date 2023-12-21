#pragma once

namespace CE::RPI
{
	enum PassAttachmentType : u32
	{
		Undefined = 0,
		Input = BIT(0),
		Output = BIT(1),
		InputOutput = Input | Output
	};

	/// @brief Describes a slot on a pass.
	struct PassSlot
	{
        /// @brief Name of the slot.
		Name name{};
        
        /// @brief Attachment type of the slot.
		PassAttachmentType attachmentType = PassAttachmentType::Undefined;

		RHI::ScopeAttachmentUsage attachmentUsage{};
        
        /// @brief Attachment's load and store action.
		RHI::AttachmentLoadStoreAction loadStoreAction{};

		RHI::UnifiedAttachmentDesc attachmentDesc{};
        
        /// @brief List of formats supported by this pass slot.
        Array<RHI::Format> formats{};
        
        /// @brief List of image dimensions supported by this pass slot.
        Array<RHI::TextureDimension> dimensions{};
	};
    
    struct AttachmentRef
    {
        Name pass{};
        Name attachment{};
    };
    
    struct PassConnection final
    {
        Name localSlot{};
        AttachmentRef attachmentRef{};
    };

    enum class AttachmentLifetime
    {
        Transient = 0,
        Imported
    };

	struct PassAttachmentDesc
	{
		/// @brief Name of the attachment.
		Name name{};

		/// @brief Lifetime of the attachment.
		AttachmentLifetime lifetime{};


	};

	struct PassBufferAttachmentDesc : PassAttachmentDesc
	{
		RHI::BufferDesc bufferDesc{};
	};
    
    struct PassImageAttachmentDesc : PassAttachmentDesc
    {
		RHI::ImageDesc imageDesc{};

		b8 generateMipChain = false;

		Array<RHI::Format> fallbackFormats{};
    };

	/// @brief Describes an attachment used by a pass. Usually used for pass outputs.
	class CORERPI_API PassAttachment final : public IntrusiveBase
	{
	public:

		PassAttachment() = default;

		PassAttachment(const PassBufferAttachmentDesc& bufferAttachmentDesc);
		PassAttachment(const PassImageAttachmentDesc& imageAttachmentDesc);
        
        Name name{};

		AttachmentLifetime lifetime{};
        
	private:

		RHI::UnifiedAttachmentDesc attachmentDesc{};

	};
    
} // namespace CE::RPI
