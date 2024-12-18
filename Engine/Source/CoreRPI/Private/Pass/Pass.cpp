#include "CoreRPI.h"

namespace CE::RPI
{

	Pass::~Pass()
	{
		
	}

	void Pass::AddAttachmentBinding(const PassAttachmentBinding& attachmentBinding)
	{
		if (attachmentBinding.slotType == PassSlotType::Input)
		{
			inputBindings.Add(attachmentBinding);
			inputBindings.Top().ownerPass = this;
		}
		else if (attachmentBinding.slotType == PassSlotType::Output)
		{
			outputBindings.Add(attachmentBinding);
			outputBindings.Top().ownerPass = this;
		}
		else if (attachmentBinding.slotType == PassSlotType::InputOutput)
		{
			inputOutputBindings.Add(attachmentBinding);
			inputOutputBindings.Top().ownerPass = this;
		}
	}

	void Pass::AddSlot(const PassSlot& slot)
	{
		slots.Add(slot);
	}

	PassSlot* Pass::FindSlot(const Name& name)
	{
		for (PassSlot& slot : slots)
		{
			if (slot.name == name)
				return &slot;
		}

		return nullptr;
	}

	PassAttachmentBinding* Pass::FindInputBinding(const Name& name)
	{
		for (int i = 0; i < inputBindings.GetSize(); i++)
		{
			if (inputBindings[i].name == name)
				return &inputBindings[i];
		}
		return nullptr;
	}

	PassAttachmentBinding* Pass::FindInputOutputBinding(const Name& name)
	{
		for (int i = 0; i < inputOutputBindings.GetSize(); i++)
		{
			if (inputOutputBindings[i].name == name)
				return &inputOutputBindings[i];
		}
		return nullptr;
	}

	PassAttachmentBinding* Pass::FindOutputBinding(const Name& name)
	{
		for (int i = 0; i < outputBindings.GetSize(); i++)
		{
			if (outputBindings[i].name == name)
				return &outputBindings[i];
		}
		return nullptr;
	}

	PassAttachmentBinding* Pass::FindBinding(const Name& name)
	{
		auto result = FindInputOutputBinding(name);
		if (result) return result;
		result = FindInputBinding(name);
		if (result) return result;
		result = FindOutputBinding(name);
		if (result) return result;
		return nullptr;
	}

	Name Pass::GetScopeId() const
	{
		return String::Format("{}_{}", GetName(), GetUuid());
	}

	Pass::Pass()
    {
		
    }

} // namespace CE::RPI
