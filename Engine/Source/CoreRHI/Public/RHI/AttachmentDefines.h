#pragma once

namespace CE::RHI
{
	typedef Name AttachmentID;
	typedef Name ScopeID;

	ENUM()
	enum class AttachmentLoadAction : u8
	{
		Load = 0,
		Clear,
		DontCare
	};
	ENUM_CLASS(AttachmentLoadAction);
    
	ENUM()
	enum class AttachmentStoreAction : u8
	{
		Store = 0,
		DontCare
	};
	ENUM_CLASS(AttachmentStoreAction);

	ENUM()
	enum class AttachmentLifetimeType
	{
		External = 0,
		Transient
	};
	ENUM_CLASS(AttachmentLifetimeType)
    
	ENUM()
	enum class AttachmentType : u8
	{
		None = 0,
		Image,
		Buffer,
		Resolve
	};
	ENUM_CLASS(AttachmentType);

	ENUM(Flags)
	enum class ScopeAttachmentAccess : u8
	{
		Undefined = 0,
		Read = BIT(0),
		Write = BIT(1),
		ReadWrite = Read | Write
	};
	ENUM_CLASS_FLAGS(ScopeAttachmentAccess)

    ///! Describes how an attachment is used by a scope.
	ENUM()
	enum class ScopeAttachmentUsage
	{
		None = 0,
		RenderTarget,
		DepthStencil,
		Copy,
		Shader,
		Resolve,
		SubpassInput,
		COUNT
	};
	ENUM_CLASS(ScopeAttachmentUsage);

	struct UnifiedScopeAttachmentDesc
	{
		UnifiedScopeAttachmentDesc() = default;

		UnifiedScopeAttachmentDesc(const ImageDescriptor& imageDesc)
			: type(AttachmentType::Image)
			, imageDesc(imageDesc)
		{

		}

		UnifiedScopeAttachmentDesc(const BufferDescriptor& bufferDesc)
			: type(AttachmentType::Buffer)
			, bufferDesc(bufferDesc)
		{

		}

		~UnifiedScopeAttachmentDesc()
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
    struct CORERHI_API AttachmentLoadStoreAction
    {
		CE_STRUCT(AttachmentLoadStoreAction)
	public:

		FIELD()
		Vec4 clearValue{};

		FIELD()
		f32 clearValueDepth = 0.0f;

		FIELD()
		u8 clearValueStencil = 0;

		FIELD()
		AttachmentLoadAction loadAction = AttachmentLoadAction::Load;

		FIELD()
		AttachmentStoreAction storeAction = AttachmentStoreAction::Store;

		FIELD()
		AttachmentLoadAction loadActionStencil = AttachmentLoadAction::Load;

		FIELD()
		AttachmentStoreAction storeActionStencil = AttachmentStoreAction::Store;
    };


    
} // namespace CE::RHI

#include "AttachmentDefines.rtti.h"
