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
    
    struct AttachmentLoadStoreAction
    {
		ClearValue clearValue{};

		AttachmentLoadAction loadAction = AttachmentLoadAction::Load;
		AttachmentStoreAction storeAction = AttachmentStoreAction::Store;

		AttachmentLoadAction loadActionStencil = AttachmentLoadAction::Load;
		AttachmentStoreAction storeActionStencil = AttachmentStoreAction::Store;
    };
    
} // namespace CE::RHI

