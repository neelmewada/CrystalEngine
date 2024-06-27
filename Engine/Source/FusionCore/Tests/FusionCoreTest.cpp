#include "FusionCoreTest.h"


namespace RenderingTests
{

    void RenderingTestWidget::Construct()
    {
        Super::Construct();

        FBrush transparentPattern = FBrush("TransparentPattern", Color::White());
        transparentPattern.SetVAlign(VAlign::Center);
        transparentPattern.SetHAlign(HAlign::Center);
        //transparentPattern.SetBrushSize(Vec2(36, 36));
        transparentPattern.SetBrushTiling(FBrushTiling::TileXY);

        FButton* openPopupBtn = nullptr;
        FButton* nativePopupBtn = nullptr;

        FAssignNew(FPopup, btnPopup)
        .BlockInteraction(false)
        .AutoClose(true)
        .Background(Color::RGBA(10, 10, 10))
        .Child(
            FNew(FVerticalStack)
            .ContentHAlign(HAlign::Center)
            .Padding(Vec4(1, 1, 1, 1) * 20)
            (
                FNew(FButton)
                .OnPressed([this]
                {
                    btnPopup->ClosePopup();
                })
                .Name("Button")
                (
                    FAssignNew(FLabel, buttonLabel)
                    .FontSize(14)
                    .Text("Click to Close!")
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

        Child(
            FNew(FStyledWidget)
            .Background(FBrush(Color::RGBA(36, 36, 36)))
            .Name("RootStyle")
            (
                FAssignNew(FVerticalStack, rootBox)
                .ContentHAlign(HAlign::Fill)
                .Padding(Vec4(10, 10, 10, 10))
                .Name("RootBox")
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
                        .FontSize(14)
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
                        .FontSize(14)
                        .Text("Open Popup")
                    ),

                    FAssignNew(FTextButton, nativePopupBtn)
                    .Name("NativePopupButton")
                    (

                    ),

                    FNew(FHorizontalStack)
                    .ContentVAlign(VAlign::Fill)
                    .Name("HStackInput")
                    (
                        FAssignNew(FTextInput, textInput)
                        .Text("This is a very long text box")
                        .Style("TextInput.Primary")
                        .MaxWidth(120),

                        FNew(FStyledWidget)
                        .Background(Color::Clear())
                        .FillRatio(1.0)
                    ),

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
        );
    }

}
