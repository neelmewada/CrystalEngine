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

    void RenderingTestWidget::Construct()
    {
        Super::Construct();

        FBrush transparentPattern = FBrush("TransparentPattern", Color::White());
        transparentPattern.SetVAlign(VAlign::Center);
        transparentPattern.SetHAlign(HAlign::Center);
        transparentPattern.SetBrushTiling(FBrushTiling::TileXY);

        FButton* openPopupBtn = nullptr;
        FTextButton* nativePopupBtn = nullptr;

        BuildPopup(btnPopup, 0);
        BuildPopup(nativePopup, 1);

        model = CreateObject<TextInputModel>(this, "DataModel");
        model->ModifyTextInCode();

        btnPopup->CalculateIntrinsicSize();
        Vec2 size1 = btnPopup->GetIntrinsicSize();
        nativePopup->CalculateIntrinsicSize();
        nativePopup->SetName("NativePopup");
        Vec2 size2 = nativePopup->GetIntrinsicSize();

        model->OnTextEdited().Bind([this]
            {
                CE_LOG(Info, All, "Text Edited via UI: {}", model->GetText());
            });
        model->OnTextModified().Bind([this]
            {
                CE_LOG(Info, All, "Text Modified via Code: {}", model->GetText());
            });

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
                                    .Background(FBrush("MinimizeIcon"))
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
                                    .Background(FBrush("MaximizeIcon"))
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
                                    .Background(FBrush("CrossIcon"))
                                    .Width(10)
                                    .Height(10)
                                    .HAlign(HAlign::Center)
                                    .VAlign(VAlign::Center)
                                )
                            )
                        )
                    ),

                    FNew(FVerticalStack)
                    .Padding(Vec4(10, 10, 10, 10))
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
                            })
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
                            .Bind_Text(BIND_PROPERTY_R(model, Text))
                        )
                    )
                )
            )
        );

        PlatformApplication::Get()->AddMessageHandler(this);
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
            maximizeIcon->Background(FBrush("MaximizeIcon"));
            this->Padding(Vec4());
        }
    }

    void RenderingTestWidget::OnWindowMaximized(PlatformWindow* window)
    {
        FNativeContext* nativeContext = static_cast<FNativeContext*>(GetContext());

        if (nativeContext->GetPlatformWindow() == window)
        {
            maximizeIcon->Background(FBrush("RestoreIcon"));
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
