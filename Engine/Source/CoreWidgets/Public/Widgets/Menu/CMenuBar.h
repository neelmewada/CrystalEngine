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

		bool IsContainer() override final { return true; }
		bool IsMenuBar() override final { return true; }

    protected:

		bool OnSubMenuItemHovered(CMenuItem* subMenuItem);

		bool IsAnySubMenuOpen();

        virtual void OnDrawGUI() override;

		virtual void HandleEvent(CEvent* event) override;
    };
    
} // namespace CE

#include "CMenuBar.rtti.h"
