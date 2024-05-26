#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API ColorPickerTool final : public CToolWindow
    {
        CE_CLASS(ColorPickerTool, CToolWindow)
    public:

        ColorPickerTool();

        virtual ~ColorPickerTool();

        static ColorPickerTool* Open();

        void SetAlphaEnabled(bool enable);

        void SetOriginalColor(const Color& original);

        void SetColor(const Color& color);

        // - Signals -

        CE_SIGNAL(OnColorSelected, Color);

        CE_SIGNAL(OnColorPickerClosed, ColorPickerTool*);

    private:

        void UpdateFields(CTextInput* excludeField = nullptr);

        void OnBeforeDestroy() override;

        void Construct() override;

        void OnClickClose() override;

        FUNCTION()
        void DoAccept();

        FUNCTION()
        void DoCancel();

        FIELD()
        Color original{};

        FIELD()
        Color value{};

        FIELD()
        CTextInput* hexInput = nullptr;

        FIELD()
        CWidget* alphaBox = nullptr;

        FIELD()
        ColorPickerGradient* gradientR = nullptr;

        FIELD()
        ColorPickerGradient* gradientG = nullptr;

        FIELD()
        ColorPickerGradient* gradientB = nullptr;

        FIELD()
        ColorPickerGradient* gradientA = nullptr;

        FIELD()
        ColorPickerGradient* gradientH = nullptr;

        FIELD()
        ColorPickerGradient* gradientS = nullptr;

        FIELD()
        ColorPickerGradient* gradientV = nullptr;

        FIELD()
        CTextInput* inputR = nullptr;

        FIELD()
        CTextInput* inputG = nullptr;

        FIELD()
        CTextInput* inputB = nullptr;

        FIELD()
        CTextInput* inputA = nullptr;

        FIELD()
        CTextInput* inputH = nullptr;

        FIELD()
        CTextInput* inputS = nullptr;

        FIELD()
        CTextInput* inputV = nullptr;

        FIELD()
        Vec3 hsv = {};

        FIELD()
        bool enableAlpha = true;

    };
    
} // namespace CE::Editor

#include "ColorPickerTool.rtti.h"