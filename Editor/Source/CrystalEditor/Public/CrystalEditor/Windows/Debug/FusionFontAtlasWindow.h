#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API FusionFontAtlasWindow : public FToolWindow
    {
        CE_CLASS(FusionFontAtlasWindow, FToolWindow)
    protected:

        FusionFontAtlasWindow();

        void Construct() override;

    public: // - Public API -

        static Ref<FusionFontAtlasWindow> Show();

    private: // - Internal -

        void OnPostComputeLayout() override;

        void OnPaint(FPainter* painter) override;

        FComboBox* fontComboBox = nullptr;
        FComboBox* fontPageComboBox = nullptr;
        FStyledWidget* background = nullptr;
        FStyledWidget* atlasViewport = nullptr;

        Vec2i imageAtlasSize = Vec2i();

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FusionFontAtlasWindow.rtti.h"
