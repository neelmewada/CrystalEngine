#include "FusionCore.h"

namespace CE
{
	FStackBoxSlot::FStackBoxSlot()
	{

	}

	FStackBox::FStackBox()
	{
		
	}

	FStackBox::~FStackBox()
	{
		
	}

	u32 FStackBox::GetSlotCount()
	{
		return m_Slots.GetSize();
	}

	FSlot* FStackBox::GetSlot(u32 index)
	{
		if (index >= m_Slots.GetSize())
			return nullptr;
		return m_Slots[index];
	}

	FWidget& FStackBox::operator+(FSlot& slot)
	{
		if (!slot.IsOfType<FStackBoxSlot>())
		{
			throw FusionException(String::Format("Invalid slot type! Expected FStackBoxSlot but got {}", slot.GetClass()->GetName()));
		}
		
		slot.SetOwner(this);

		FStackBoxSlot* cast = (FStackBoxSlot*)&slot;
		m_Slots.Add(cast);

		return *this;
	}

	FWidget& FStackBox::operator+(const FSlot& slot)
	{
		return operator+(const_cast<FSlot&>(slot));
	}

	bool FStackBox::RemoveSlot(FSlot* slot)
	{
		if (!slot->IsOfType<FStackBoxSlot>())
			return false;

		bool exists = m_Slots.Remove((FStackBoxSlot*)slot);
		if (!exists)
			return false;

		DetachSubobject(slot);
		slot->SetOwner(nullptr);

		if (slot->GetChild())
		{
			slot->SetChild(nullptr);
		}

		return true;
	}

	void FStackBox::Construct()
	{
		Super::Construct();

		
	}

}
