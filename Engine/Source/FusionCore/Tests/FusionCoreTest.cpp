#include "FusionCoreTest.h"


namespace RenderingTests
{
	void RenderingTestWidget::BuildPopup(FPopup*& outPopup, int index)
	{
        FPopup* popup = nullptr;

        FAssignNew(FPopup, popup)
        .BlockInteraction(false)
        .AutoClose(true)
        .Background(Color::RGBA(10, 10, 10))
        .Child(
            FNew(FVerticalStack)
            .ContentHAlign(HAlign::Center)
            .Padding(Vec4(1, 1, 1, 1) * 20)
            (
                FNew(FButton)
                .OnPressed([this, popup]
                {
                    popup->ClosePopup();
                })
                .Name("Button")
                (
                    FAssignNew(FLabel, buttonLabel)
                    .FontSize(14)
                    .Text(String::Format("Click to Close! ({})", index))
                ),

                FNew(FStyledWidget)
                .Background(Color::Blue())
                .MinHeight(26)
                (
                    FNew(FLabel)
                    .FontSize(20)
                    .Text("This is a long label")
                )
            )
        );

        outPopup = popup;
	}

    inline String MakeText(int index)
	{
        return String::Format("Line no. {}: This is the first sentence. Followed by a really long second sentence that has a total of 14 words. This is the 3rd sentence in the text line. And this is the last sentence.", index);
	}

    inline String MakeShortText(int index)
	{
        return String::Format("Line no. {}: This is the first sentence.", index);
	}

    static Color scrollColors[] = {Color::Yellow(), Color::Green(), Color::Cyan(), Color::Blue(), Color::Red()};

    void RenderingTestWidget::Construct()
    {
        Super::Construct();

        FBrush transparentPattern = FBrush("/Engine/Resources/Icons/TransparentPattern", Color::White());
        transparentPattern.SetVAlign(VAlign::Center);
        transparentPattern.SetHAlign(HAlign::Center);
        transparentPattern.SetBrushTiling(FBrushTiling::TileXY);

        FButton* openPopupBtn = nullptr;
        FTextButton* nativePopupBtn = nullptr;

        PlatformApplication::Get()->AddMessageHandler(this);

        BuildPopup(btnPopup, 0);
        BuildPopup(nativePopup, 1);

        model = CreateObject<TextInputModel>(this, "DataModel");

        btnPopup->CalculateIntrinsicSize();
        nativePopup->CalculateIntrinsicSize();
        nativePopup->SetName("NativePopup");

        model->SetComboItems({ "Combo Item 0", "Combo Item 1", "Combo Item 2" });

        Child(
            FAssignNew(FStyledWidget, borderWidget)
            .Background(FBrush(Color::RGBA(36, 36, 36)))
            .BorderWidth(1.0f)
            .BorderColor(Color::RGBA(15, 15, 15))
            .Padding(Vec4(1, 1, 1, 1))
            .Name("RootStyle")
            (
                FAssignNew(FVerticalStack, rootBox)
                .ContentHAlign(HAlign::Fill)
                .Name("RootBox")
                (
                    FNew(FTitleBar)
                    .Background(Color::RGBA(26, 26, 26))
                    .Height(40)
                    .HAlign(HAlign::Fill)
                    (
                        FNew(FOverlayStack)
                        .VAlign(VAlign::Fill)
                        .HAlign(HAlign::Fill)
                        (
                            FNew(FHorizontalStack)
                            .ContentVAlign(VAlign::Center)
                            .HAlign(HAlign::Fill)
                            .VAlign(VAlign::Fill)
                            (
                                FNew(FTerminalWidget)
                                .FillRatio(1.0f),

                                FNew(FLabel)
                                .FontSize(15)
                                .Text("Widget Demo")
                                .HAlign(HAlign::Center)
                                .VAlign(VAlign::Center),

                                FNew(FTerminalWidget)
                                .FillRatio(1.0f)
                            ),

                            FNew(FHorizontalStack)
                            .HAlign(HAlign::Fill)
                            .VAlign(VAlign::Fill)
                            (
                                FNew(FTerminalWidget)
                                .FillRatio(1.0f),

                                FNew(FButton)
                                .OnPressed([this]
                                    {
                                        static_cast<FNativeContext*>(GetContext())->Minimize();
                                    })
                                .Padding(Vec4(17, 8, 17, 8))
                                .Name("WindowMinimizeButton")
                                .Style("Button.WindowControl")
                                .VAlign(VAlign::Top)
                                (
                                    FNew(FImage)
                                    .Background(FBrush("/Engine/Resources/Icons/MinimizeIcon"))
                                    .Width(11)
                                    .Height(11)
                                    .HAlign(HAlign::Center)
                                    .VAlign(VAlign::Center)
                                ),

                                FNew(FButton)
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
                        )
                    ),

                    // Window Content Begins

                    FNew(FVerticalStack)
                    .Padding(Vec4(10, 10, 10, 10))
                    .Name("ContentVStack")
                    (
                        FNew(FHorizontalStack)
                        .ContentVAlign(VAlign::Fill)
                        .Name("HStack1")
                        (
                            FNew(FStyledWidget)
                            .Background(transparentPattern)
                            .BackgroundShape(FRoundedRectangle(2.5f, 5, 7.5f, 10))
                            .FillRatio(1.0f)
                            .MinWidth(60)
                            .MinHeight(30)
                        )
                        .Margin(Vec4(0, 0, 0, 5)),

                        FAssignNew(FButton, button)
                        .OnPressed([this]
                            {
                                buttonLabel->Text(String::Format("Click Count {}", ++hitCounter));
                            })
                        .Name("Button")
                        (
                            FAssignNew(FLabel, buttonLabel)
                            .FontSize(13)
                            .Text("Click Count 0")
                        ),

                        FAssignNew(FButton, openPopupBtn)
                        .OnPressed([this, openPopupBtn]
                            {
                                GetContext()->PushLocalPopup(btnPopup, openPopupBtn->GetGlobalPosition() + Vec2(0, openPopupBtn->GetComputedSize().y));
                            })
                        .Name("PopupButton")
                        (
                            FNew(FLabel)
                            .FontSize(13)
                            .Text("Open Popup")
                        ),

                        FAssignNew(FTextButton, nativePopupBtn)
                        .Text("Open Native Popup")
                        .OnPressed([this, nativePopupBtn]
                            {
                                GetContext()->PushNativePopup(nativePopup, nativePopupBtn->GetGlobalPosition() + Vec2(0, nativePopupBtn->GetComputedSize().y));
                            })
                        .Name("NativePopupButton"),

                        FAssignNew(FTextInput, textInput)
                        .Text("This is a very long text box")
                        .Style("TextInput.Primary")
                        .MaxWidth(120)
                        .HAlign(HAlign::Left)
                        .Margin(Vec4(0, 0, 0, 5)),

                        FNew(FHorizontalStack)
                        .ContentVAlign(VAlign::Center)
                        .Name("HStack2")
                        (
                            FAssignNew(FStyledWidget, subWidget)
                            .Background(FBrush(Color::Green()))
                            .BackgroundShape(FRectangle())
                            .FillRatio(1.0f)
                            .MinWidth(60)
                            .MinHeight(15),

                            FNew(FStyledWidget)
                            .Background(FBrush(Color::Cyan()))
                            .BackgroundShape(FRectangle())
                            .FillRatio(2.0f)
                            .MinWidth(60)
                            .MinHeight(40),

                            FNew(FStyledWidget)
                            .Background(FBrush(Color::Yellow()))
                            .BackgroundShape(FRectangle())
                            .FillRatio(1.0f)
                            .MinWidth(60)
                            .MinHeight(25)
                        ),

                        FNew(FLabel)
                        .FontSize(18)
                        .Text("Bindings Demo")
                        .HAlign(HAlign::Left),

                        FNew(FHorizontalStack)
                        .ContentVAlign(VAlign::Center)
                        .ContentHAlign(HAlign::Center)
                        (
                            FAssignNew(FComboBox, comboBox)
                            .Bind_Items(BIND_PROPERTY_R(model, ComboItems))
                            .MaxWidth(120)
                            .HAlign(HAlign::Left)
                            .Margin(Vec4(0, 0, 0, 5)),

                            FNew(FTextButton)
                            .Text("Add Item")
                            .OnPressed([this]
                                {
                                    auto items = model->GetComboItems();
                                    items.Add({ String::Format("Combo Item {}", items.GetSize()) });
                                    model->SetComboItems(items);
                                }),

                            FNew(FTextButton)
                            .Text("Print Memory Footprint")
                            .OnPressed([this]
                                {
                                    model->UpdateMemoryFootprint();
                                }),

                            FNew(FLabel)
                            .Bind_Text(BIND_PROPERTY(model, MemoryFootprint, [this] { return String::Format("Fusion Memory: {} KB", model->GetMemoryFootprint() / 1024); }, nullptr))
                            .FontSize(14)
                        ),

                        FNew(FHorizontalStack)
                        .ContentVAlign(VAlign::Center)
                        (
							FAssignNew(FTextInput, modelTextInput)
							.Bind_Text(BIND_PROPERTY_RW(model, Text))
                            .FontSize(13)
                            .Width(180)
                            .Margin(Vec4(0, 0, 10, 0)),

                            FNew(FTextButton)
                            .FontSize(13)
                            .Text("Randomize")
                            .OnPressed([this]
	                            {
	                                model->ModifyTextInCode();
	                            }),

                            FAssignNew(FLabel, modelDisplayLabel)
                            .FontSize(13)
                            .Bind_Text(BIND_PROPERTY_R(model, Text)),

                            FNew(FTextButton)
                            .Text("Show/Hide SplitBox")
                            .OnPressed([this]
	                            {
	                                splitBox->Enabled(!splitBox->Enabled());
	                            })
                        ),
                        
                        FAssignNew(FSplitBox, splitBox)
                        .Direction(FSplitDirection::Horizontal)
                        .HAlign(HAlign::Fill)
                        .Height(40)
                        (
							FNew(FStyledWidget)
                            .Background(Color::Green())
                            .FillRatio(0.25f),

                            FNew(FStyledWidget)
                            .Background(Color::Yellow())
                            .FillRatio(0.25f),

                            FNew(FStyledWidget)
                            .Background(Color::Cyan())
                            .FillRatio(0.5f)
                        ),

                        FNew(FTabView)
                        .TabItems(
							FNew(FLabelTabItem)
                            .Text("Tab 1")
                            .ContentWidget(
								// Tab 1 Content - BEGIN
								FNew(FScrollBox)
		                        .ScrollBarBrush(Color::RGBA(255, 255, 255, 100))
		                        .ScrollBarHoverBrush(Color::RGBA(255, 255, 255, 140))
		                        .VerticalScroll(true)
		                        .HorizontalScroll(true)
		                        .Height(80)
		                        (
									FNew(FVerticalStack)
		                            .ContentHAlign(HAlign::Left)
		                            .VAlign(VAlign::Top)
		                            .HAlign(HAlign::Left)
		                            (
										FForEach { 5,
		                                    [this] (int index) -> FWidget&
		                                    {
		                                        return
		                                        FNew(FVerticalStack)
		                                        .ContentHAlign(HAlign::Left)
		                                        (
		                                            FNew(FStyledWidget)
		                                            .Background(scrollColors[index])
		                                            .Padding(Vec4(1, 1, 1, 1) * 5.0f)
		                                            (
		                                                FNew(FLabel)
		                                                .FontSize(16)
		                                                .Foreground(Color::RGBA(140, 140, 140))
		                                                .Text(MakeText(index))
		                                            )
		                                        );
		                                    }
										},

		                                FNew(FVerticalStack)
		                                .ContentHAlign(HAlign::Left)
		                                (
		                                    FNew(FStyledWidget)
		                                    .Background(Color::Red())
		                                    .Padding(Vec4(1, 1, 1, 1) * 5.0f)
		                                    (
		                                        FNew(FTextButton)
		                                        .FontSize(16)
		                                        .Text("Click Here!")
		                                    )
		                                )
		                            )
		                        )
                                // Tab 1 Content - END
                            )
                            .Name("Tab1"),

                            FNew(FLabelTabItem)
                            .Text("Tab 2")
                            .ContentWidget(
								// Tab 2 - Content
	                            FNew(FStyledWidget)
	                            .Background(Color::Cyan())
	                            .HAlign(HAlign::Fill)
	                            .VAlign(VAlign::Fill)
	                            .Padding(Vec4(1, 1, 1, 1) * 10)
	                            (
		                            FNew(FLabel)
		                            .FontSize(18)
		                            .Text("This is second tab")
                                    .Foreground(Color::Black())
	                            )
                            )
                            .Name("Tab2")
                        )
                        .MinHeight(64)
                        .Margin(Vec4(0, 5, 0, 0)),

                        FNew(FMenuBar)
                        .Content(
							FNew(FMenuItem)
                            .Text("File")
                            .SubMenu(
                                FNew(FMenuPopup)
                                .Name("FileMenu")
                                .As<FMenuPopup>()
                                .Gap(0)
                                .Content(
                                    FNew(FMenuItem)
                                    .Text("New"),

                                    FNew(FMenuItem)
                                    .Text("Open"),

                                    FNew(FMenuItem)
                                    .Text("Open As..."),

                                    FNew(FMenuItem)
                                    .Text("Open Recent...")
                                    .SubMenu(
										FNew(FMenuPopup)
                                        .Gap(0)
                                        .Content(
                                            FNew(FMenuItem)
                                            .Text("Project 1"),

                                            FNew(FMenuItem)
                                            .Text("Project 2"),

                                            FNew(FMenuItem)
                                            .Text("Project 3"),

                                            FNew(FMenuItem)
                                            .Text("Project 4")
                                        )
                                        .Name("RecentsMenu")
                                        .As<FMenuPopup>()
                                    ),

                                    FNew(FMenuItem)
                                    .Text("Save"),

                                    FNew(FMenuItem)
                                    .Text("Save As..."),

                                    FNew(FMenuItem)
                                    .Text("Exit")
                                )
                            )
                            .Name("FileMenuItem"),

                            FNew(FMenuItem)
                            .Text("Edit")
                            .Name("EditMenuItem"),

                            FNew(FMenuItem)
                            .Text("Help")
                            .Name("HelpMenuItem")
                        )
                        .Margin(Vec4(0, 10, 0, 0))
                        .Name("MenuBar")
                    )
                )
            )
        );
    }

    void RenderingTestWidget::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        PlatformApplication::Get()->RemoveMessageHandler(this);
    }

    void RenderingTestWidget::OnWindowRestored(PlatformWindow* window)
    {
        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());

        if (nativeContext->GetPlatformWindow() == window)
        {
            maximizeIcon->Background(FBrush("/Engine/Resources/Icons/MaximizeIcon"));
            this->Padding(Vec4());
        }
    }

    void RenderingTestWidget::OnWindowMaximized(PlatformWindow* window)
    {
        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());

        if (nativeContext->GetPlatformWindow() == window)
        {
            maximizeIcon->Background(FBrush("/Engine/Resources/Icons/RestoreIcon"));
            this->Padding(Vec4(1, 1, 1, 1) * 4.0f);
        }
    }

    void RenderingTestWidget::OnWindowExposed(PlatformWindow* window)
    {
        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());

        if (nativeContext->GetPlatformWindow() == window && !window->IsMaximized())
        {
            OnWindowRestored(window);
        }
    }

}
