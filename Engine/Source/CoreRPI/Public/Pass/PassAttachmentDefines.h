#pragma once

namespace CE::RPI
{
	constexpr u32 PassAttachmentBindingCountMax = 32;
	constexpr u32 PassInputBindingCountMax = PassAttachmentBindingCountMax;
	constexpr u32 PassOutputBindingCountMax = PassAttachmentBindingCountMax;
	constexpr u32 PassInputOutputBindingCountMax = PassAttachmentBindingCountMax;

	ENUM()
	enum class PassSlotType : u32
	{
		Undefined = 0,
		Input = BIT(0),
		Output = BIT(1),
		InputOutput = Input | Output
	};
	ENUM_CLASS_FLAGS(PassSlotType);

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
		PassSlotType slotType = PassSlotType::Undefined;

		FIELD()
		Name shaderInputName{};

		FIELD()
		RHI::ScopeAttachmentUsage attachmentUsage{};

		FIELD()
		b8 isArray = false;

		/// @brief Attachment's load and store action.
		FIELD()
		RHI::AttachmentLoadStoreAction loadStoreAction{};

		/// @brief List of formats supported by this pass slot.
		FIELD()
        Array<RHI::Format> formats{};

		/// @brief List of image dimensions supported by this pass slot.
		FIELD()
        FixedArray<RHI::Dimension, 4> dimensions = { RHI::Dimension::Dim2D };
	};

	//! @brief Name of the PassAttachment to use as reference
	typedef Name PassAttachmentRef;


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

		/// @brief Fixed size values for (width, height, depth) dimensions.
		FIELD()
		Vec3i fixedSizes = Vec3i(0, 0, 1);

		bool IsFixedSize() const
		{
			return fixedSizes.x != 0 && fixedSizes.y != 0 && fixedSizes.z != 0;
		}
	};

	STRUCT()
	struct CORERPI_API ImageAttachmentDescriptor
	{
		CE_STRUCT(ImageAttachmentDescriptor)
	public:

		FIELD()
		RHI::TextureBindFlags bindFlags{};

		FIELD()
		RHI::Dimension dimension = RHI::Dimension::Dim2D;

		FIELD()
		u16 arrayLayers = 1;

		FIELD()
		u16 mipCount = 1;

		FIELD()
		RHI::Format format = RHI::Format::Undefined;

		FIELD()
		u8 sampleCount = 1;
	};

	STRUCT()
	struct CORERPI_API BufferAttachmentDescriptor
	{
		CE_STRUCT(BufferAttachmentDescriptor)
	public:

		FIELD()
		RHI::BufferBindFlags bindFlags{};

		FIELD()
		u64 byteSize = 0;


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
		RHI::AttachmentLifetimeType lifetime{};

		FIELD()
		PassAttachmentSizeSource sizeSource{};

	};

	STRUCT()
    struct CORERPI_API PassImageAttachmentDesc : PassAttachmentDesc
    {
		CE_STRUCT(PassImageAttachmentDesc, PassAttachmentDesc)
	public:

		FIELD()
		ImageAttachmentDescriptor imageDescriptor{};

		FIELD()
		Name formatSource;

		FIELD()
		Array<RHI::Format> fallbackFormats{};
    };


	STRUCT()
	struct CORERPI_API PassBufferAttachmentDesc : public PassAttachmentDesc
	{
		CE_STRUCT(PassBufferAttachmentDesc, PassAttachmentDesc)
	public:

		FIELD()
		BufferAttachmentDescriptor bufferDescriptor{};


	};

	struct CORERPI_API UnifiedAttachmentDescriptor
	{
		UnifiedAttachmentDescriptor()
        {
			memset(this, 0, sizeof(*this));
			type = RHI::AttachmentType::None;
        }

		UnifiedAttachmentDescriptor(const ImageAttachmentDescriptor& imageDesc)
			: type(RHI::AttachmentType::Image)
			, imageDesc(imageDesc)
		{

		}

		UnifiedAttachmentDescriptor(const BufferAttachmentDescriptor& bufferDesc)
			: type(RHI::AttachmentType::Buffer)
			, bufferDesc(bufferDesc)
		{

		}

		UnifiedAttachmentDescriptor(const UnifiedAttachmentDescriptor& copy);

		UnifiedAttachmentDescriptor& operator=(const UnifiedAttachmentDescriptor& copy);

		~UnifiedAttachmentDescriptor();

		RHI::AttachmentType type = RHI::AttachmentType::None;

		union {
			ImageAttachmentDescriptor imageDesc;
			BufferAttachmentDescriptor bufferDesc;
		};
	};

} // namespace CE::RPI

#include "PassAttachmentDefines.rtti.h"
