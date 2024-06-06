#include "FusionCore.h"

namespace CE
{

    FWidget::FWidget()
    {
        
    }

    FWidget::~FWidget()
    {
        
    }

    FWidget& FWidget::operator+(const FLayoutSlot& slot)
    {
        return operator+(const_cast<FLayoutSlot&>(slot));
    }

    FWidget& FWidget::operator+(FLayoutSlot& slot)
    {
        return *this;
    }

    void FWidget::AddLayoutSlot(const FLayoutSlot& slot)
    {
        operator+(slot);
    }

    bool FWidget::RemoveLayoutSlot(FLayoutSlot* slot)
    {
        return false;
    }

    void FWidget::DestroyLayoutSlot(FLayoutSlot* slot)
    {
        if (!slot)
            return;

        if (RemoveLayoutSlot(slot))
        {
            slot->Destroy();
        }
    }

    FFusionContext* FWidget::GetContext()
    {
        if (!context && parent && parent->GetOwner())
        {
            context = parent->GetOwner()->GetContext();
        }
        return context;
    }

    Vec2 FWidget::PrecomputeLayoutSize()
    {
        return Vec2();
    }

    void FWidget::OnAfterConstruct()
    {
	    Super::OnAfterConstruct();

        Construct();
    }

    void FWidget::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (parent != nullptr && parent->GetOwner() != nullptr)
        {
            auto parentSlot = parent;
            FWidget* ownerWidget = parentSlot->GetOwner();
            ownerWidget->DestroyLayoutSlot(parentSlot);
        }
    }

    void FWidget::Construct()
    {
        
    }

} // namespace CE

