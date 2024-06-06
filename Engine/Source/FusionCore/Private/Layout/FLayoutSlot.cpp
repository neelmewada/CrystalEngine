#include "FusionCore.h"

namespace CE
{
	void FLayoutSlot::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

        SetChild(nullptr);
	}

	void FLayoutSlot::SetOwner(FWidget* owner)
	{
        m_Owner = owner;

        if (owner)
        {
            owner->AttachSubobject(this);
        }
	}

	void FLayoutSlot::SetChild(FWidget* newChild)
	{
        if (this->m_Child && newChild == nullptr)
        {
            // Remove this slot as parent from the child widget
            this->m_Child->parent = nullptr;
        }
        this->m_Child = newChild;
	}

	FLayoutSlot::Self& FLayoutSlot::Child(FWidget& child)
    {
        this->m_Child = &child;
        child.parent = this;
        return *this;
    }

} // namespace CE
