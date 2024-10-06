#include "EditorCore.h"

namespace CE::Editor
{

    EditorDockspace::EditorDockspace()
    {

    }

    void EditorDockspace::AddDockTab(EditorDockTab* tab)
    {
        if (dockedEditors.Exists(tab))
            return;

        dockedEditors.Add(tab);

        UpdateTabWell();

        if (selectedTab < 0)
        {
            SelectTab(tab);
        }
    }

    void EditorDockspace::SelectTab(EditorDockTabItem* tabItem)
    {
	    for (int i = 0; i < tabItems.GetSize(); ++i)
	    {
		    if (tabItems[i] == tabItem)
		    {
                selectedTab = i;
                content->Child(*dockedEditors[selectedTab]);

                return;
		    }
	    }

        ApplyStyle();
    }

    void EditorDockspace::SelectTab(EditorDockTab* tab)
    {
	    for (int i = 0; i < dockedEditors.GetSize(); ++i)
	    {
		    if (dockedEditors[i] == tab)
		    {
                selectedTab = i;
                content->Child(*tab);

                return;
		    }
	    }

        ApplyStyle();
    }

    void EditorDockspace::UpdateTabWell()
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
	        }
            else
            {
                EditorDockTabItem* child = nullptr;

                FAssignNew(EditorDockTabItem, child)
                .Text(dockedEditors[i]->Title())
                .VAlign(VAlign::Fill)
            	;

                tabWell->AddChild(child);
                tabItems.Add(child);
            }
        }

        ApplyStyle();
    }

    void EditorDockspace::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FStyledWidget, borderWidget)
            .Background(FBrush(Color::RGBA(36, 36, 36)))
            .BorderWidth(1.0f)
            .BorderColor(Color::RGBA(15, 15, 15))
            .Padding(Vec4(1, 1, 1, 1))
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            .Name("RootStyle")
            (
                FAssignNew(FVerticalStack, rootBox)
                .ContentHAlign(HAlign::Fill)
                .ContentVAlign(VAlign::Top)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                .Name("RootBox")
                (
                    // - Title Bar Begin -

                    FAssignNew(FTitleBar, titleBar)
                    .Background(FBrush(Color::RGBA(26, 26, 26)))
                    .Height(45)
                    .HAlign(HAlign::Fill)
                    (
                        FNew(FHorizontalStack)
                        .ContentVAlign(VAlign::Center)
                        .HAlign(HAlign::Fill)
                        .VAlign(VAlign::Fill)
                        .Name("TestHStack")
                        (
                            // - Logo -

                            FAssignNew(FImage, logo)
                            .Background(FBrush("/Engine/Resources/Icons/LogoGray"))
                            .Width(32)
                            .Height(32)
                            .VAlign(VAlign::Center)
                            .Margin(Vec4(10, 0, 10, 0)),

                            // - Tabs -

                            FAssignNew(FHorizontalStack, tabWell)
                            .ContentHAlign(HAlign::Left)
                            .VAlign(VAlign::Fill)
                            .FillRatio(1.0f)
                            .Name("TabWell")
                            .Padding(Vec4(5, 5, 0, 0)),

                            // - Window Controls -

                            FAssignNew(FButton, minimizeButton)
                            .OnPressed([this]
                                {
                                    static_cast<FNativeContext*>(GetContext())->Minimize();
                                })
                            .Padding(Vec4(17, 8, 17, 8))
                            .Name("WindowMinimizeButton")
                            .Style("Button.WindowControl")
                            .VAlign(VAlign::Top)
                            (
                                FAssignNew(FImage, minimizeIcon)
                                .Background(FBrush("/Engine/Resources/Icons/MinimizeIcon"))
                                .Width(11)
                                .Height(11)
                                .HAlign(HAlign::Center)
                                .VAlign(VAlign::Center)
                            ),

                            FAssignNew(FButton, maximizeButton)
                            .OnPressed([this]
                                {
                                    FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());
                                    if (nativeContext->IsMaximized())
                                    {
                                        nativeContext->Restore();
                                    }
                                    else
                                    {
                                        nativeContext->Maximize();
                                    }
                                })
                            .Padding(Vec4(17, 8, 17, 8))
                            .Name("WindowMaximizeButton")
                            .Style("Button.WindowControl")
                            .VAlign(VAlign::Top)
                            (
                                FAssignNew(FImage, maximizeIcon)
                                .Background(FBrush("/Engine/Resources/Icons/MaximizeIcon"))
                                .Width(11)
                                .Height(11)
                                .HAlign(HAlign::Center)
                                .VAlign(VAlign::Center)
                            ),

                            FNew(FButton)
                            .OnPressed([this]
                                {
                                    GetContext()->QueueDestroy();
                                })
                            .Padding(Vec4(18, 8, 18, 8))
                            .Name("WindowCloseButton")
                            .Style("Button.WindowClose")
                            .VAlign(VAlign::Top)
                            (
                                FNew(FImage)
                                .Background(FBrush("/Engine/Resources/Icons/CrossIcon"))
                                .Width(10)
                                .Height(10)
                                .HAlign(HAlign::Center)
                                .VAlign(VAlign::Center)
                            )
                        )
                    ),

                    // - Title Bar End -

                    FAssignNew(FStyledWidget, content)
                    .Padding(Vec4(0, 0, 0, 0))
                    .HAlign(HAlign::Fill)
                    .VAlign(VAlign::Fill)
                    .FillRatio(1.0f)
                    .Name("Content")
                )

            ) // End of Child
        );
    }
    
}

