#pragma once

namespace CE::RPI
{
	ENUM()
	enum PassAttachmentType : u32
	{
		Undefined = 0,
		Input = BIT(0),
		Output = BIT(1),
		InputOutput = Input | Output
	};

	STRUCT()
	struct PassSlot
	{
		CE_STRUCT(PassSlot)
	public:

		FIELD()
		Name name{}; /// @brief Name of the slot.

		FIELD()
		PassAttachmentType attachmentType = PassAttachmentType::Undefined; /// @brief Attachment type of the slot.

		FIELD()
		RHI::AttachmentLoadStoreAction loadStoreAction{}; /// @brief Attachment's load and store action.

	};
    
} // namespace CE::RPI

#include "PassAttachment.rtti.h"
