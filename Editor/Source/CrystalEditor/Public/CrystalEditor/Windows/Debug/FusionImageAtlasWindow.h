#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API FusionImageAtlasWindow : public FToolWindow
    {
        CE_CLASS(FusionImageAtlasWindow, FToolWindow)
    protected:

        FusionImageAtlasWindow();

        void Construct() override;

    public: // - Public API -

        static Ref<FusionImageAtlasWindow> Show();

    private: // - Internal -

        void OnPostComputeLayout() override;

        void OnPaint(FPainter* painter) override;

        FComboBox* comboBox = nullptr;
        FStyledWidget* background = nullptr;
        FStyledWidget* atlasViewport = nullptr;

        Vec2i imageAtlasSize = Vec2i();

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FusionImageAtlasWindow.rtti.h"
