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

		RHI::AttachmentLoadStoreAction loadStoreAction{};

	};
    
} // namespace CE::RPI
