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
        
		FIELD()
		Name name{}; /// @brief Name of the slot.
        
		FIELD()
		PassSlotType slotType = PassSlotType::Undefined; /// @brief Attachment type of the slot.

		FIELD()
		Name shaderInputName{};

		FIELD()
		RHI::ScopeAttachmentUsage attachmentUsage{};
        
		FIELD()
		RHI::AttachmentLoadStoreAction loadStoreAction{}; /// @brief Attachment's load and store action.
        
		FIELD()
        Array<RHI::Format> formats{}; /// @brief List of formats supported by this pass slot.
        
		FIELD()
        Array<RHI::Dimension> dimensions{}; /// @brief List of image dimensions supported by this pass slot.
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

		inline bool IsValid() const
		{
			return pass.IsValid() && attachment.IsValid();
		}
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

		inline bool IsValid() const
		{
			return localSlot.IsValid() && attachmentRef.IsValid();
		}
    };

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
		Vec3i fixedSizes = Vec3i(0, 0, 0);
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
    struct CORERPI_API PassImageAttachmentDesc : PassAttachmentDesc
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

	STRUCT()
	struct CORERPI_API BufferDescriptor
	{
		CE_STRUCT(BufferDescriptor)
	public:

		FIELD()
		RHI::BufferBindFlags bindFlags{};

		FIELD()
		u64 byteSize = 0;


	};

	STRUCT()
	struct CORERPI_API PassBufferAttachmentDesc : public PassAttachmentDesc
	{
		CE_STRUCT(PassBufferAttachmentDesc, PassAttachmentDesc)
	public:

		FIELD()
		BufferDescriptor bufferDescriptor{};


	};

	struct UnifiedAttachmentDescriptor
	{
		UnifiedAttachmentDescriptor() = default;

		UnifiedAttachmentDescriptor(const ImageDescriptor& imageDesc)
			: type(RHI::AttachmentType::Image)
			, imageDesc(imageDesc)
		{

		}

		UnifiedAttachmentDescriptor(const BufferDescriptor& bufferDesc)
			: type(RHI::AttachmentType::Buffer)
			, bufferDesc(bufferDesc)
		{

		}

		UnifiedAttachmentDescriptor(const UnifiedAttachmentDescriptor& copy)
		{
			if (copy.type == AttachmentType::Image)
			{
				if (copy.type == type)
				{
					imageDesc = copy.imageDesc;
				}
				else // Copy is a buffer
				{
					imageDesc.~ImageDescriptor();
					memset(&bufferDesc, 0, sizeof(bufferDesc));
					bufferDesc = copy.bufferDesc;
				}
				type = copy.type;
			}
			else if (copy.type == AttachmentType::Buffer)
			{
				if (copy.type == type)
				{
					bufferDesc = copy.bufferDesc;
				}
				else // Copy is an image
				{
					bufferDesc.~BufferDescriptor();
					memset(&imageDesc, 0, sizeof(imageDesc));
					imageDesc = copy.imageDesc;
				}
				type = copy.type;
			}
		}

		UnifiedAttachmentDescriptor& operator=(const UnifiedAttachmentDescriptor& copy)
		{
			if (copy.type == AttachmentType::Image)
			{
				if (copy.type == type)
				{
					imageDesc = copy.imageDesc;
				}
				else // Copy is a buffer
				{
					imageDesc.~ImageDescriptor();
					memset(&bufferDesc, 0, sizeof(bufferDesc));
					bufferDesc = copy.bufferDesc;
				}
				type = copy.type;
			}
			else if (copy.type == AttachmentType::Buffer)
			{
				if (copy.type == type)
				{
					bufferDesc = copy.bufferDesc;
				}
				else // Copy is an image
				{
					bufferDesc.~BufferDescriptor();
					memset(&imageDesc, 0, sizeof(imageDesc));
					imageDesc = copy.imageDesc;
				}
				type = copy.type;
			}
			return *this;
		}

		~UnifiedAttachmentDescriptor()
		{
			if (type == AttachmentType::Image)
			{
				imageDesc.~ImageDescriptor();
			}
			else
			{
				bufferDesc.~BufferDescriptor();
			}
		}

		AttachmentType type = AttachmentType::Image;

		union {
			ImageDescriptor imageDesc{};
			BufferDescriptor bufferDesc;
		};
	};

	STRUCT()
	struct CORERPI_API PassData
	{
		CE_STRUCT(PassData)
	public:

		FIELD()
		Name drawListTag{};

		FIELD()
		Name viewTag{};

	};

	/// @brief Describes the pass to create as a subpass of current pass.
	STRUCT()
	struct CORERPI_API PassRequest
	{
		CE_STRUCT(PassRequest)
	public:

		/// @brief Name of the pass after instantiation.
		FIELD()
		Name passName{};

		/// @brief Name of pass template we will instantiate from.
		FIELD()
		Name passDefinition{};

		/// @brief Connections of the instantiated pass.
		FIELD()
		Array<PassConnection> connections{};

		/// @brief List of child passes through pass requests.
		FIELD()
		Array<PassRequest> childPasses{};

	};

	STRUCT()
	struct CORERPI_API PassDefinition
	{
		CE_STRUCT(PassDefinition)
	public:

		FIELD()
		Name name{};

		FIELD()
		Name passClass{};

		FIELD()
		Array<PassSlot> slots{};

		FIELD()
		Array<PassConnection> connections{};

		FIELD()
		Array<PassImageAttachmentDesc> imageAttachments{};

		FIELD()
		Array<PassBufferAttachmentDesc> bufferAttachments{};

		FIELD()
		PassData passData{};

		inline PassSlot* FindSlot(const Name& slotName) const
		{
			for (int i = 0; i < slots.GetSize(); i++)
			{
				if (slots[i].name == slotName)
					return const_cast<PassSlot*>(&slots[i]);
			}
			return nullptr;
		}

		inline int GetPassSlotIndex(const Name& slotName) const
		{
			return slots.IndexOf([&](const PassSlot& slot) { return slot.name == slotName; });
		}
	};
    
} // namespace CE::RPI

#include "PassAttachmentDefines.rtti.h"
