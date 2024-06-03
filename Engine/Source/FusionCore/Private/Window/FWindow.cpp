#include "FusionCore.h"

namespace CE
{

    FWindow::FWindow()
    {
        m_ChildSlot = CreateDefaultSubobject<FWindowSlot>("ChildSlot");
        m_ChildSlot->SetOwner(this);
    }

    void FWindow::Construct()
    {
        Super::Construct();
        
    }
    
}

