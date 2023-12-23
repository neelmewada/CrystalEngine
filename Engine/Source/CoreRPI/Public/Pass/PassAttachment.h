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
	ENUM_CLASS_FLAGS(PassAttachmentType);

	/// @brief Describes a slot on a pass.
	STRUCT()
	struct CORERPI_API PassSlot
	{
		CE_STRUCT(PassSlot)
	public:

        /// @brief Name of the slot.
		FIELD()
		Name name{};
        
        /// @brief Attachment type of the slot.
		FIELD()
		PassAttachmentType slotType = PassAttachmentType::Undefined;

		FIELD()
		RHI::ScopeAttachmentUsage attachmentUsage{};
        
        /// @brief Attachment's load and store action.
		FIELD()
		RHI::AttachmentLoadStoreAction loadStoreAction{};
        
        /// @brief List of formats supported by this pass slot.
		FIELD()
        Array<RHI::Format> formats{};
        
        /// @brief List of image dimensions supported by this pass slot.
		FIELD()
        Array<RHI::Dimension> dimensions{};
	};
    
	STRUCT()
    struct CORERPI_API PassAttachmentRef
    {
		CE_STRUCT(PassAttachmentRef)
	public:

		FIELD()
        Name pass{};

		FIELD()
        Name attachment{};
    };
    
	STRUCT()
    struct CORERPI_API PassConnection
    {
		CE_STRUCT(PassConnection)
	public:

		FIELD()
        Name localSlot{};

		FIELD()
		PassAttachmentRef attachmentRef{};
    };

	ENUM()
    enum class AttachmentLifetime
    {
        Transient = 0,
        Imported
    };
	ENUM_CLASS(AttachmentLifetime);

	STRUCT()
	struct CORERPI_API PassAttachmentSizeSource
	{
		CE_STRUCT(PassAttachmentSizeSource)
	public:

		FIELD()
		PassAttachmentRef source{};

		/// @brief Size multipliers for (width, height, depth) from source attachment.
		FIELD()
		Vec3 sizeMultipliers = Vec3(1, 1, 1);
	};

	STRUCT()
	struct CORERPI_API PassAttachmentDesc
	{
		CE_STRUCT(PassAttachmentDesc)
	public:

		/// @brief Name of the attachment.
		FIELD()
		Name name{};

		/// @brief Lifetime of the attachment.
		FIELD()
		AttachmentLifetime lifetime{};

		FIELD()
		PassAttachmentSizeSource sizeSource{};

	};

	STRUCT()
	struct CORERPI_API BufferDescriptor
	{
		CE_STRUCT(BufferDescriptor)
	public:

		FIELD()
		RHI::BufferBindFlags bindFlags{};


	};

	STRUCT()
	struct CORERPI_API PassBufferAttachmentDesc : PassAttachmentDesc
	{
		CE_STRUCT(PassBufferAttachmentDesc, PassAttachmentDesc)
	public:

		
	};

	STRUCT()
	struct CORERPI_API ImageDescriptor
	{
		CE_STRUCT(ImageDescriptor)
	public:

		FIELD()
		RHI::TextureBindFlags bindFlags{};

		FIELD()
		RHI::Dimension dimension = RHI::Dimension::Dim2D;

		FIELD()
		u16 arraySize = 1;

		FIELD()
		u16 mipCount = 1;

		FIELD()
		RHI::Format format = RHI::Format::Undefined;


	};
    
	STRUCT()
    struct PassImageAttachmentDesc : PassAttachmentDesc
    {
		CE_STRUCT(PassImageAttachmentDesc, PassAttachmentDesc)
	public:

		FIELD()
		ImageDescriptor imageDescriptor{};

		FIELD()
		b8 generateFullMipChain = false;

		FIELD()
		Array<RHI::Format> fallbackFormats{};
    };
    
} // namespace CE::RPI

#include "PassAttachment.rtti.h"
