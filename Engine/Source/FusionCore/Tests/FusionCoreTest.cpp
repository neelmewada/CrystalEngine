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

        btnPopup->CalculateIntrinsicSize();
        Vec2 size1 = btnPopup->GetIntrinsicSize();
        nativePopup->CalculateIntrinsicSize();
        nativePopup->SetName("NativePopup");
        Vec2 size2 = nativePopup->GetIntrinsicSize();

        Child(
            FNew(FStyledWidget)
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
                                .Text("Window Title")
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

                                    })
                                .Padding(Vec4(20, 8, 20, 8))
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

                                    })
                                .Padding(Vec4(20, 8, 20, 8))
                                .Name("WindowMaximizeButton")
                                .Style("Button.WindowControl")
                                .VAlign(VAlign::Top)
                                (
                                    FNew(FImage)
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

                        FAssignNew(FComboBox, comboBox)
                        .Items(
                            "Items 0",
                            "Items 1",
                            "Items 2",
                            "Items 3"
                        )
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
                        )
                    )
                )
            )
        );
    }

}
