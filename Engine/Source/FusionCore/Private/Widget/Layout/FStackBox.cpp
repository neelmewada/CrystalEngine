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

	void FStackBox::PrecomputeLayoutSize()
	{
		precomputedSize = Vec2();
		precomputedSize.width = m_Padding.left + m_Padding.right;
		precomputedSize.height = m_Padding.top + m_Padding.bottom;

		Vec2 baseSize = Vec2();

		for (FStackBoxSlot* slot : m_Slots)
		{
			FWidget* child = slot->GetChild();
			if (!child)
				continue;

			const Vec4& padding = slot->GetPadding();
			
			child->PrecomputeLayoutSize();

			Vec2 childSize = child->GetComputedLayoutSize();

			childSize.width = Math::Clamp(childSize.width, slot->GetMinWidth(), slot->GetMaxWidth());
			childSize.height = Math::Clamp(childSize.height, slot->GetMinHeight(), slot->GetMaxHeight());
			
			slot->computedSize = Vec2(childSize.width, childSize.height);
			
			switch (m_Direction)
			{
			case FStackBoxDirection::Horizontal:
				baseSize.width += childSize.width + padding.left + padding.right;
				baseSize.height = Math::Max(baseSize.height, childSize.height + padding.top + padding.bottom);
				break;
			case FStackBoxDirection::Vertical:
				baseSize.height += childSize.height + padding.top + padding.bottom;
				baseSize.width = Math::Max(baseSize.width, childSize.width + padding.left + padding.right);
				break;
			}
		}

		precomputedSize.width += baseSize.width;
		precomputedSize.height += baseSize.height;
	}

	void FStackBox::PerformLayout(Vec2 availableSize)
	{
		

		for (FStackBoxSlot* slot : m_Slots)
		{
			
		}
	}

	void FStackBox::Construct()
	{

	}

}
