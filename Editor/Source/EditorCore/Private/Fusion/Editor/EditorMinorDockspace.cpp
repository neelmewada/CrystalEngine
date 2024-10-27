#include "EditorCore.h"

namespace CE::Editor
{

    EditorMinorDockspace::EditorMinorDockspace()
    {

    }

    void EditorMinorDockspace::AddDockTab(EditorDockTab* tab)
    {
        if (tab == nullptr || dockedEditors.Exists(tab))
            return;

        tab->dockspace = this;

        dockedEditors.Add(tab);

        UpdateTabWell();

        if (selectedTab < 0)
        {
            SelectTab(tab);
        }
    }

    void EditorMinorDockspace::SelectTab(EditorDockTabItem* tabItem)
    {
        for (int i = 0; i < tabItems.GetSize(); ++i)
        {
            if (tabItems[i] == tabItem)
            {
                selectedTab = i;
                content->Child(*dockedEditors[selectedTab]);

                tabItems[i]->isActive = true;
            }
            else
            {
                tabItems[i]->isActive = false;

                AttachSubobject(dockedEditors[i]);
            }
        }

        ApplyStyle();
    }

    void EditorMinorDockspace::SelectTab(EditorDockTab* tab)
    {
        for (int i = 0; i < dockedEditors.GetSize(); ++i)
        {
            if (dockedEditors[i] == tab)
            {
                selectedTab = i;
                content->Child(*tab);

                tabItems[i]->isActive = true;
            }
            else
            {
                tabItems[i]->isActive = false;

                AttachSubobject(dockedEditors[i]);
            }
        }

        ApplyStyle();
    }

    void EditorMinorDockspace::UpdateTabWell()
    {
        while (tabWell->GetChildCount() > dockedEditors.GetSize())
        {
            FWidget* child = tabWell->GetChild(tabWell->GetChildCount() - 1);
            tabWell->RemoveChild(child);
            child->Destroy();

            EditorDockTabItem* tabItem = tabItems.GetLast();
            tabItems.Remove(tabItem);
            tabItem->Destroy();
        }

        for (int i = 0; i < dockedEditors.GetSize(); ++i)
        {
            if (i < (int)tabItems.GetSize())
            {
                EditorDockTabItem& child = *tabItems[i];

                child
                    .Text(dockedEditors[i]->Title())
                    ;

                child.dockTab = dockedEditors[i];
            }
            else
            {
                EditorDockTabItem* child = nullptr;

                FAssignNew(EditorDockTabItem, child)
					.ContentPadding(Vec4(1, 0, 1, 0) * 10)
                    .Text(dockedEditors[i]->Title())
                    .VAlign(VAlign::Fill)
                    ;

                child->dockTab = dockedEditors[i];

                tabWell->AddChild(child);
                tabItems.Add(child);
            }
        }

        ApplyStyle();
    }

    void EditorMinorDockspace::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FVerticalStack, rootBox)
            .ContentHAlign(HAlign::Fill)
            .ContentVAlign(VAlign::Top)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            .Name("RootBox")
            (
                FAssignNew(FStyledWidget, titleBar)
                .Background(Color::Black())
                .HAlign(HAlign::Fill)
                .Height(30)
                (
                    FAssignNew(FHorizontalStack, tabWell)
                    .ContentHAlign(HAlign::Left)
                    .VAlign(VAlign::Fill)
                    .Name("TabWell")
                ),

                FAssignNew(FStyledWidget, content)
                .Padding(Vec4(0, 0, 0, 0))
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                .FillRatio(1.0f)
                .Name("Content")
            )
        );
    }

}

