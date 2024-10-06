#pragma once

namespace CE::Editor
{
    class EditorDockTab;


    CLASS()
    class EDITORCORE_API EditorDockTabItem : public FStyledWidget
    {
        CE_CLASS(EditorDockTabItem, FStyledWidget)
    public:

        // - Public API -

        bool SupportsMouseEvents() const override { return true; }

    protected:

        EditorDockTabItem();

        void Construct() override;

        void HandleEvent(FEvent* event) override;

        void OnPostComputeLayout() override;

        FLabel* label = nullptr;

        bool isActive = false;
        bool isHovered = false;

    public:

        FUSION_PROPERTY_WRAPPER(Text, label);

        FUSION_WIDGET;
        friend class EditorDockspaceStyle;
    };

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

    public: // - Fusion Properties - 

        FUSION_PROPERTY(String, Title);

        FUSION_WIDGET;
    };
    
}

#include "EditorDockTab.rtti.h"
