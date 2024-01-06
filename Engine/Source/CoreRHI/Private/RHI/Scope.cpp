#include "CoreRHI.h"

namespace CE::RHI
{

    Scope::Scope(const ScopeDescriptor& desc)
    {
		id = desc.id;
		queueClass = desc.queueClass;
    }

	Scope::~Scope()
	{
		for (auto attachment : attachments)
		{
			delete attachment;
		}
	}

	void Scope::AddScopeAttachment(ScopeAttachment* attachment)
	{
		attachments.Add(attachment);
	}

	bool Scope::ScopeAttachmentExists(const Name& id)
	{
		for (auto attachment : attachments)
		{
			if (attachment->id == id)
				return true;
		}
		return false;
	}

	ScopeAttachment* Scope::FindScopeAttachment(const Name& id)
	{
		for (auto attachment : attachments)
		{
			if (attachment->id == id)
				return attachment;
		}
		return nullptr;
	}
    
} // namespace CE::RHI
