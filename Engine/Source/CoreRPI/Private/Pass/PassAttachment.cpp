#include "CoreRPI.h"

namespace CE::RPI
{

	PassAttachment::PassAttachment(const PassBufferAttachmentDesc& desc)
		: attachmentDesc(desc.bufferDesc)
	{
		name = desc.name;
		lifetime = desc.lifetime;
	}

	PassAttachment::PassAttachment(const PassImageAttachmentDesc& desc)
		: attachmentDesc(desc.imageDesc)
	{
		name = desc.name;
		lifetime = desc.lifetime;
	}


} // namespace CE::RPI
