#include "Fusion.h"

namespace CE
{
    
    FToolWindow::FToolWindow()
    {
	    
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
                .Name("RootBox")
                (
                    FNew(FTitleBar)
                    .Background(Color::RGBA(26, 26, 26))
                    .Height(40)
                    .HAlign(HAlign::Fill)
                )
            )
        );

    }

} // namespace CE
