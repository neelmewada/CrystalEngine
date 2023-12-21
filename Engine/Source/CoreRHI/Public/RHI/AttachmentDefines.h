#pragma once

namespace CE::RHI
{
	enum class AttachmentLoadAction : u8
	{
		Load = 0,
		Clear,
		DontCare
	};
    
	enum class AttachmentStoreAction : u8
	{
		Store = 0,
		DontCare
	};
    
	enum class AttachmentType : u8
	{
		None = 0,
		Image,
		Buffer,
		Resolve
	};

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

	struct UnifiedAttachmentDesc
	{
		UnifiedAttachmentDesc() = default;

		UnifiedAttachmentDesc(const ImageDesc& imageDesc)
			: type(AttachmentType::Image)
			, imageDesc(imageDesc)
		{

		}

		UnifiedAttachmentDesc(const BufferDesc& bufferDesc)
			: type(AttachmentType::Buffer)
			, bufferDesc(bufferDesc)
		{

		}

		~UnifiedAttachmentDesc()
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
    
    struct AttachmentLoadStoreAction
    {
		ClearValue clearValue{};

		AttachmentLoadAction loadAction = AttachmentLoadAction::Load;
		AttachmentStoreAction storeAction = AttachmentStoreAction::Store;

		AttachmentLoadAction loadActionStencil = AttachmentLoadAction::Load;
		AttachmentStoreAction storeActionStencil = AttachmentStoreAction::Store;
    };
    
} // namespace CE::RHI

