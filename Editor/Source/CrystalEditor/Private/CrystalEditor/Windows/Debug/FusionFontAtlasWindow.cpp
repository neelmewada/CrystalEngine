#include "CrystalEditor.h"

namespace CE::Editor
{
    static WeakRef<FusionFontAtlasWindow> instance = nullptr;

    FusionFontAtlasWindow::FusionFontAtlasWindow()
    {

    }

    void FusionFontAtlasWindow::Construct()
    {
        Super::Construct();

        const CE::Name gridSmall = "/Engine/Resources/Images/GridSmall";

        FBrush gridBg = FBrush(gridSmall);
        gridBg.SetBrushSize(Vec2(16, 16));
        gridBg.SetBrushTiling(FBrushTiling::TileXY);
        gridBg.SetImageFit(FImageFit::Fill);

        (*this)
            .Title("Fusion Font Atlas")
            .MinimizeEnabled(true)
            .MaximizeEnabled(true)
            .ContentPadding(Vec4(1, 1, 1, 1) * 10.0f)
			.ContentGap(10)
            .Content(
                FNew(FHorizontalStack)
                .ContentHAlign(HAlign::Left)
                .ContentVAlign(VAlign::Center)
                .Height(35)
                .HAlign(HAlign::Fill)
                (
                    FAssignNew(FComboBox, comboBox)
                    .OnSelectionChanged([this](FComboBox*)
                    {
                        int selectionIndex = comboBox->GetSelectedItemIndex();


                    })
                    .Width(60)
                ),

                FNew(FOverlayStack)
                .ClipChildren(true)
                .FillRatio(1.0f)
                .HAlign(HAlign::Fill)
                (
                    FAssignNew(FStyledWidget, background)
                    .Background(gridBg)
                    .VAlign(VAlign::Fill)
                    .HAlign(HAlign::Fill),

                    FAssignNew(FStyledWidget, atlasViewport)
                    .VAlign(VAlign::Fill)
                    .HAlign(HAlign::Fill)
                )
            );

        comboBox->SelectItem(0);
    }

    void FusionFontAtlasWindow::OnPostComputeLayout()
    {
        Super::OnPostComputeLayout();

    }

    void FusionFontAtlasWindow::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        imageAtlasSize = painter->GetImageAtlasSize();
    }

    Ref<FusionFontAtlasWindow> FusionFontAtlasWindow::Show()
    {
        if (Ref<FusionFontAtlasWindow> lock = instance.Lock())
        {
            FNativeContext* nativeContext = static_cast<FNativeContext*>(lock->GetContext());
            PlatformWindow* window = nativeContext->GetPlatformWindow();
            window->Show();
            return lock;
        }

        PlatformWindowInfo info{
            .maximised = false,
            .fullscreen = false,
            .resizable = true,
            .hidden = false,
            .windowFlags = PlatformWindowFlags::DestroyOnClose
        };

        Ref<FusionFontAtlasWindow> window = (Ref<FusionFontAtlasWindow>)FusionApplication::Get()->CreateNativeWindow(
            "FusionFontAtlasWindow", 
            "Fusion Font Atlas",
            800, 600,
            Self::StaticClass(), info);

        instance = window;

        window->ShowWindow();

        return window;
    }

}

