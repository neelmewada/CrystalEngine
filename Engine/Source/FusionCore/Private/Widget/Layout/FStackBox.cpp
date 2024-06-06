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

	FLayoutSlot* FStackBox::GetSlot(u32 index)
	{
		if (index >= m_Slots.GetSize())
			return nullptr;
		return m_Slots[index];
	}

	FWidget& FStackBox::operator+(FLayoutSlot& slot)
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

	FWidget& FStackBox::operator+(const FLayoutSlot& slot)
	{
		return operator+(const_cast<FLayoutSlot&>(slot));
	}

	bool FStackBox::RemoveLayoutSlot(FLayoutSlot* slot)
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

	Vec2 FStackBox::PrecomputeLayoutSize()
	{
		precomputedSize = Vec2();
		precomputedSize.width += m_Padding.left + m_Padding.right;
		precomputedSize.height += m_Padding.top + m_Padding.bottom;

		for (FStackBoxSlot* slot : m_Slots)
		{
			FWidget* child = slot->GetChild();
			if (!child)
				continue;

			const Vec4& padding = slot->GetPadding();

			Vec2 childSize = child->PrecomputeLayoutSize();

			switch (m_Direction)
			{
			case FStackBoxDirection::Horizontal:
				precomputedSize.height = Math::Max(precomputedSize.height, childSize.height + padding.top + padding.bottom);
				break;
			case FStackBoxDirection::Vertical:
				precomputedSize.width = Math::Max(precomputedSize.width, childSize.width + padding.left + padding.right);
				break;
			}
		}

		return precomputedSize;
	}

	void FStackBox::Construct()
	{

	}

}
