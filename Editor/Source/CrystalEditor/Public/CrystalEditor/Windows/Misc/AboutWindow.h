#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API AboutWindow : public FToolWindow
    {
        CE_CLASS(AboutWindow, FToolWindow)
    protected:

        AboutWindow();

        void Construct() override;

        void OnBeforeDestroy() override;

    public: // - Public API -

        static AboutWindow* Show();

    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "AboutWindow.rtti.h"
