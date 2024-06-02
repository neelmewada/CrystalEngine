#include "FusionCore.h"

namespace CE
{

    FWidget::FWidget()
    {
        
    }

    FWidget::~FWidget()
    {
        
    }

    FWidget& FWidget::operator+(const FSlot& slot)
    {
        return operator+(const_cast<FSlot&>(slot));
    }

    FWidget& FWidget::operator+(FSlot& slot)
    {
        return *this;
    }

    void FWidget::AddSlot(const FSlot& slot)
    {
        operator+(slot);
    }

    bool FWidget::RemoveSlot(FSlot* slot)
    {
        return false;
    }

    void FWidget::DestroySlot(FSlot* slot)
    {
        if (!slot)
            return;

        if (RemoveSlot(slot))
        {
            slot->Destroy();
        }
    }

    void FWidget::OnAfterConstruct()
    {
	    Super::OnAfterConstruct();

        Construct();
    }

    void FWidget::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (m_Parent != nullptr && m_Parent->GetOwner() != nullptr)
        {
            auto parentSlot = m_Parent;
            FWidget* ownerWidget = parentSlot->GetOwner();
            ownerWidget->DestroySlot(parentSlot);
        }
    }

    void FWidget::Construct()
    {
        
    }

} // namespace CE

