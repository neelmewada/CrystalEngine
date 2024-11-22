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

        bool IsActive() const { return isActive; }
        bool IsHovered() const { return isHovered; }

    protected:

        EditorDockTabItem();

        void Construct() override;

        void HandleEvent(FEvent* event) override;

        void OnPostComputeLayout() override;

    private:

        FLabel* label = nullptr;
        FHorizontalStack* content = nullptr;
        WeakRef<EditorDockTab> dockTab = nullptr;
        FImageButton* closeButton = nullptr;

        bool isActive = false;
        bool isHovered = false;

    public:

        FUSION_PROPERTY_WRAPPER(Text, label);

        FUSION_PROPERTY_WRAPPER2(Padding, content, ContentPadding);

        FUSION_PROPERTY_WRAPPER2(Enabled, closeButton, CloseButtonEnabled);

        FUSION_WIDGET;
        friend class EditorDockspace;
        friend class EditorMinorDockspace;
        friend class EditorDockTab;
        friend class EditorDockspaceStyle;
        friend class EditorMinorDockspaceStyle;
    };
    
}

#include "EditorDockTabItem.rtti.h"
