#pragma once

namespace CE::Editor
{
    class EditorDockTab;
    class EditorDockTabItem;

    CLASS()
    class EDITORCORE_API EditorDockspace : public EditorWindow
    {
        CE_CLASS(EditorDockspace, EditorWindow)
    public:

        // - Public API -

        void AddDockTab(EditorDockTab* tab);

        void SelectTab(EditorDockTabItem* tabItem);
        void SelectTab(EditorDockTab* tab);

    protected:

        EditorDockspace();

        void UpdateTabWell();

        void Construct() override;

        FStyledWidget* borderWidget = nullptr;
        FVerticalStack* rootBox = nullptr;
        FImage* maximizeIcon = nullptr;
        FImage* minimizeIcon = nullptr;
        FStyledWidget* content = nullptr;
        FTitleBar* titleBar = nullptr;
        FLabel* titleBarLabel = nullptr;

        FImage* logo = nullptr;
        FButton* minimizeButton = nullptr;
        FButton* maximizeButton = nullptr;

        FHorizontalStack* tabWell = nullptr;

        Array<EditorDockTab*> dockedEditors;
        Array<EditorDockTabItem*> tabItems;
        int selectedTab = -1;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER(Background, borderWidget);
        FUSION_PROPERTY_WRAPPER(BorderWidth, borderWidget);
        FUSION_PROPERTY_WRAPPER(BorderColor, borderWidget);

        FUSION_PROPERTY_WRAPPER2(Background, titleBar, TitleBarBackground);

        FUSION_WIDGET;
        friend class EditorDockspaceStyle;
    };
    
}

#include "EditorDockspace.rtti.h"
