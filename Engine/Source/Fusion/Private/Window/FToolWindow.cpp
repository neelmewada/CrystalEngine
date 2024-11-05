#include "Fusion.h"

namespace CE
{
    
    FToolWindow::FToolWindow()
    {
	    
    }

    void FToolWindow::SetMaximizeButton(bool interactable)
    {
        Color tintColor = interactable ? Color::White() : Color::RGBA(255, 255, 255, 80);

        maximizeIcon->Background(maximizeIcon->Background().WithTint(tintColor));
        maximizeButton->SetInteractionEnabled(interactable);
    }

    void FToolWindow::SetMinimizeButton(bool interactable)
    {
        Color tintColor = interactable ? Color::White() : Color::RGBA(255, 255, 255, 80);

        minimizeIcon->Background(minimizeIcon->Background().WithTint(tintColor));
        minimizeButton->SetInteractionEnabled(interactable);
    }

    void FToolWindow::Construct()
    {
        Super::Construct();

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
                .ContentVAlign(VAlign::Top)
                .Name("RootBox")
                (
                    // - Title Bar Begin -

                    FAssignNew(FTitleBar, titleBar)
                    .Background(FBrush(Color::RGBA(26, 26, 26)))
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

                                FAssignNew(FLabel, titleBarLabel)
                                .FontSize(15)
                                .Text("Tool Window")
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
                        )
                    ),

                    // - Title Bar End -

                    // - Body Begin -

                    FAssignNew(FStackBox, content)
                    .Direction(FStackBoxDirection::Vertical)
                    .Padding(Vec4(10, 10, 10, 10))
                    .VAlign(VAlign::Fill)
                    .FillRatio(1.0f)
                    .Name("ContentVStack")

                    // - Body End -
                )
            )
        );

        this->Style("ToolWindow");
    }

    FToolWindow::Self& FToolWindow::MinimizeEnabled(bool enabled)
    {
        minimizeButton->Enabled(enabled);
        return *this;
    }

    FToolWindow::Self& FToolWindow::MinimizeInteractable(bool interactable)
    {
        SetMinimizeButton(interactable);
        return *this;
    }

    FToolWindow::Self& FToolWindow::MaximizeEnabled(bool enabled)
    {
        maximizeButton->Enabled(enabled);
        return *this;
    }

    FToolWindow::Self& FToolWindow::MaximizeInteractable(bool interactable)
    {
        SetMaximizeButton(interactable);
        return *this;
    }

} // namespace CE
