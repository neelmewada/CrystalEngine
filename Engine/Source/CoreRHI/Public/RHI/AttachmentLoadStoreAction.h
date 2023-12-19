#pragma once

namespace CE::RHI
{
	ENUM()
	enum class AttachmentLoadAction : u8
	{
		Load = 0,
		Clear,
		DontCare
	};

	ENUM()
	enum class AttachmentStoreAction : u8
	{
		Store = 0,
		DontCare
	};

	ENUM()
	enum class AttachmentType : u8
	{
		None = 0,
		Image,
		Buffer,
		Resolve
	};

	STRUCT()
    struct AttachmentLoadStoreAction
    {
		CE_STRUCT(AttachmentLoadStoreAction)
	public:

		FIELD()
		ClearValue clearValue{};

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

#include "AttachmentLoadStoreAction.rtti.h"
