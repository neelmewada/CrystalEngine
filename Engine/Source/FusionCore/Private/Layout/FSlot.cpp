#include "FusionCore.h"

namespace CE
{
	void FSlot::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

        SetChild(nullptr);
	}

	void FSlot::SetOwner(FWidget* owner)
	{
        m_Owner = owner;

        if (owner)
        {
            owner->AttachSubobject(this);
        }
	}

	void FSlot::SetChild(FWidget* newChild)
	{
        if (this->m_Child && newChild == nullptr)
        {
            // Remove this slot as parent from the child widget
            this->m_Child->m_Parent = nullptr;
        }
        this->m_Child = newChild;
	}

	FSlot::Self& FSlot::Child(FWidget& child)
    {
        this->m_Child = &child;
        child.m_Parent = this;
        return *this;
    }

} // namespace CE
