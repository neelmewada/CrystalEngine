#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorMinorDockspace : public FStyledWidget
    {
        CE_CLASS(EditorMinorDockspace, FStyledWidget)
    public:

        // - Public API -

        void AddDockTab(EditorDockTab* tab);
        void RemoveDockTab(EditorDockTab* tab);

        void SelectTab(EditorDockTabItem* tabItem);
        void SelectTab(EditorDockTab* tab);

    protected:

        EditorMinorDockspace();

        void Construct() override;

        void UpdateTabWell();

        FVerticalStack* rootBox = nullptr;
        FStyledWidget* titleBar = nullptr;
        FHorizontalStack* tabWell = nullptr;
        FStyledWidget* content = nullptr;

        Array<EditorDockTab*> dockedEditors;
        Array<EditorDockTabItem*> tabItems;
        int selectedTab = -1;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER2(Background, titleBar, TitleBarBackground);

        template<typename... TArgs> requires TMatchAllBaseClass<EditorDockTab, TArgs...>::Value and (sizeof...(TArgs) > 0)
        Self& DockTabs(TArgs&... dockTabs)
        {
            std::initializer_list<EditorDockTab*> list = { &dockTabs... };

            for (EditorDockTab* dockTab : list)
            {
                AddDockTab(dockTab);
            }

            return *this;
        }

        FUSION_WIDGET;
        friend class EditorMinorDockspaceStyle;
    };
    
}

#include "EditorMinorDockspace.rtti.h"
