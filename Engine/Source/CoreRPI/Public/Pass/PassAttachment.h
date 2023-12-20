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

	struct PassSlot
	{
        /// @brief Name of the slot.
		Name name{};
        
        /// @brief Attachment type of the slot.
		PassAttachmentType attachmentType = PassAttachmentType::Undefined;
        
        /// @brief Attachment's load and store action.
		RHI::AttachmentLoadStoreAction loadStoreAction{};
        
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
    
    struct PassImageAttachmentDesc
    {
        /// @brief Name of the attachment.
        Name name{};
        
        /// @brief Lifetime of the image attachment.
        AttachmentLifetime lifetime{};
        
        
    };

	/// @brief Describes an attachment used by a pass.
	class CORERPI_API PassAttachment final : public IntrusiveBase
	{
	public:

		PassAttachment() = default;
        
        Name name{};
        
	};
    
} // namespace CE::RPI
