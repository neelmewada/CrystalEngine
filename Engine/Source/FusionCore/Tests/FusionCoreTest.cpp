#include "FusionCoreTest.h"

namespace LayoutTests
{

    void TerminalWidget::Construct()
    {
	    
    }

    struct FBuilder
    {
	    
    };

    void LayoutTestWidget::Construct()
    {
        bool cond = false;

        Child(
            FAssignNew(FVerticalStack, rootBox)
            .ContentHAlign(HAlign::Fill)
            .Padding(Vec4(5, 5, 5, 5))
            .Name("RootBox")
            (
                FAssignNew(FHorizontalStack, hStack1)
                .IfTrue(true, [&](FHorizontalStack& self)
                    {
                        self
                            .Padding(Vec4(5, 2.5f, 5, 2.5f));
                    })
                .ContentVAlign(VAlign::Center)
                .Name("HStack1")
                (
                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(30),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Fill)
                    .MinHeight(15)
                    .FillRatio(1.0),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Top)
                    .MinHeight(15)
                    .FillRatio(2.0),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Center)
                    .MinHeight(15)
                    .FillRatio(1.0),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Bottom)
                    .MinHeight(15)
                    .FillRatio(1.0)
                ),

                FAssignNew(FHorizontalStack, hStack2)
                .ContentVAlign(VAlign::Center)
                .Padding(Vec4())
                .Name("HStack2")
                (
                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10)
                    .Margin(Vec4(0, 0, 5, 0)),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(30)
                    .FillRatio(1.0)
                    .Margin(Vec4(0, 0, 5, 0)),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10)
                    .FillRatio(3.0)
                ),

                FAssignNew(FHorizontalStack, hStack3)
                .FillRatio(1.0f)
                .HAlign(HAlign::Center)
                .Name("HStack3")
                (
                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(30)
                )
            )
        );
    }

}

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
                        .Text("Type your text")
                        .Style("TextInput.Primary")
                        .MinWidth(150),

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
