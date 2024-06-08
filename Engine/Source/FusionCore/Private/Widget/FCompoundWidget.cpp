#include "FusionCore.h"

namespace CE
{

    FCompoundWidget::FCompoundWidget()
    {

    }

    bool FCompoundWidget::TryAddChild(FWidget* child)
    {
        if (!child)
            return false;

        m_Child = child;
        child->parent = this;
        return true;
    }

}

