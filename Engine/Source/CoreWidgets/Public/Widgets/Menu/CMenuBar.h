#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CMenuBar : public CWidget
    {
        CE_CLASS(CMenuBar, CWidget)
    public:

        CMenuBar();

        virtual ~CMenuBar();

    protected:

        virtual void OnDrawGUI() override;

    };
    
} // namespace CE

#include "CMenuBar.rtti.h"
