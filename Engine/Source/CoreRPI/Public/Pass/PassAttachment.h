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
        
	};

    struct AttachmentRef
    {
        Name pass{};
        Name attachmentSlot{};
    };
    
    struct PassConnection final
    {
        Name localSlot{};
        AttachmentRef attachmentRef{};
    };
    
	/// @brief Describes an attachment used by a pass.
	class CORERPI_API PassAttachment final : public IntrusiveBase
	{
	public:

		PassAttachment() = default;

		

	};
    
} // namespace CE::RPI
