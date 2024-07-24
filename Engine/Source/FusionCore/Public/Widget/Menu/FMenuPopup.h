#pragma once

namespace CE
{
    class FMenuItem;

    CLASS()
    class FUSIONCORE_API FMenuPopup : public FPopup
    {
        CE_CLASS(FMenuPopup, FPopup)
    public:

        FMenuPopup();

        u32 GetMenuItemCount() const { return menuItems.GetSize(); }

        FMenuItem* GetMenuItem(u32 index) const { return menuItems[index]; }

    protected:

        void Construct() override;

        FIELD()
        Array<FMenuItem*> menuItems;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FMenu.rtti.h"
