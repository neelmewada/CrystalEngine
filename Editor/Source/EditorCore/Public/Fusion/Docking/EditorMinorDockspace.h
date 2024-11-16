#pragma once

namespace CE::Editor
{
    class EditorDockTabItem;

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

        FVerticalStack* GetRootBox() const { return rootBox; }

        int GetTabItemCount() const { return tabItems.GetSize(); }
        EditorDockTabItem* GetTabItem(int index) const { return tabItems[index]; }

    protected:

        EditorMinorDockspace();

        void Construct() override;

        void UpdateTabWell();

    private:

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
