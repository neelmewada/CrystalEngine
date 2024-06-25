#include "FusionCoreTest.h"


namespace RenderingTests
{

    void RenderingTestWidget::Construct()
    {
        Super::Construct();

        Child
    	(
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
                        .Background(FBrush(Color::Green()))
                        .BackgroundShape(FRectangle())
                        .FillRatio(1.0f)
                        .MinWidth(60)
                        .MinHeight(30)
                        .Name("GreenWidget")
                    )
                    .Margin(Vec4(0, 0, 0, 5)),

                    FAssignNew(FButton, button)
                    .OnPressed([this]
		                {
                            buttonLabel->Text(String::Format("Click Count {}", ++hitCounter));
		                })
                    .ClipShape(FRectangle())
                    .Style("Button.Primary")
                    .Name("Button")
                    (
                        FAssignNew(FLabel, buttonLabel)
                        .FontSize(14)
                        .Text("Click Count 0")
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
