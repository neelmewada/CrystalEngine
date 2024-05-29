#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CLabelButton : public CLabel
    {
        CE_CLASS(CLabelButton, CLabel)
    public:

        CLabelButton();

    protected:

        void Construct() override;

    };
    
} // namespace CE::Widgets

#include "CLabelButton.rtti.h"