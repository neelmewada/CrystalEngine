#pragma once

namespace CE::Widgets
{
	
    CLASS()
    class COREWIDGETS_API CContextMenu : public CMenu
    {
        CE_CLASS(CContextMenu, CMenu)
    public:

        CContextMenu();
        virtual ~CContextMenu();

    protected:

        virtual void OnDrawGUI() override;


    };
    
} // namespace CE

#include "CContextMenu.rtti.h"
