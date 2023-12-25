#pragma once

namespace CE::RHI
{
	typedef Name AttachmentID;

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
	enum class AttachmentType : u8
	{
		None = 0,
		Image,
		Buffer,
		Resolve
	};
	ENUM_CLASS(AttachmentType);

	ENUM()
	enum class ScopeAttachmentUsage
	{
		None = 0,
		RenderTarget,
		DepthStencil,
		Shader,
		Resolve,
		SubpassInput,
		COUNT
	};
	ENUM_CLASS(ScopeAttachmentUsage);

	struct UnifiedScopeAttachmentDesc
	{
		UnifiedScopeAttachmentDesc() = default;

		UnifiedScopeAttachmentDesc(const ImageDesc& imageDesc)
			: type(AttachmentType::Image)
			, imageDesc(imageDesc)
		{

		}

		UnifiedScopeAttachmentDesc(const BufferDesc& bufferDesc)
			: type(AttachmentType::Buffer)
			, bufferDesc(bufferDesc)
		{

		}

		~UnifiedScopeAttachmentDesc()
		{
			if (type == AttachmentType::Image)
			{
				imageDesc.~ImageDesc();
			}
			else
			{
				bufferDesc.~BufferDesc();
			}
		}

		AttachmentType type = AttachmentType::Image;

		union {
			ImageDesc imageDesc{};
			BufferDesc bufferDesc;
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
