#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API SampleWidgetWindow : public FToolWindow
    {
        CE_CLASS(SampleWidgetWindow, FToolWindow)
    protected:

        SampleWidgetWindow();

        void Construct() override;

        void OnBeforeDestroy() override;

    public: // - Public API -

        static SampleWidgetWindow* Show();

    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SampleWidgetWindow.rtti.h"
