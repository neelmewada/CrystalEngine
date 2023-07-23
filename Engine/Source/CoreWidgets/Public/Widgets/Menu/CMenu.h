#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CMenu : public CWidget
    {
        CE_CLASS(CMenu, CWidget)
    public:

        CMenu();

        virtual ~CMenu();

    protected:

        virtual void OnDrawGUI() override;

    };
    
} // namespace CE

#include "CMenu.rtti.h"
