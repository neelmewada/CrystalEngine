#include "FusionCore.h"

namespace CE
{

    FCompoundWidget::FCompoundWidget()
    {
        m_ChildSlot = CreateDefaultSubobject<FCompoundWidgetSlot>("CompoundWidgetSlot");
        m_ChildSlot->SetOwner(this);
    }

    bool FCompoundWidget::RemoveSlot(FSlot* slot)
    {
        return false;
    }

    void FCompoundWidget::Construct()
    {
        Super::Construct();


    }
}

