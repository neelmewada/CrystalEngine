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
    
    typedef Array<PassSlot> PassSlotList;

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
    
    
    
} // namespace CE::RPI

#include "PassAttachment.rtti.h"
