#pragma once

namespace CE::Editor
{

    CLASS()
    class EDITORCORE_API EditorDockTab : public EditorWindow
    {
        CE_CLASS(EditorDockTab, EditorWindow)
    public:

        // - Public API -

        virtual bool CanBeClosed() const { return true; }

    protected:

        EditorDockTab();

        void Construct() override;

        EditorMenuBar* menuBar = nullptr;
        EditorToolBar* toolBar = nullptr;
        FStyledWidget* content = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(String, Title);

        FUSION_PROPERTY_WRAPPER2(Enabled, toolBar, ToolBarEnabled);

        FUSION_WIDGET;
    };
    
}

#include "EditorDockTab.rtti.h"
