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

        // - Events -

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
        CColorPicker* colorMap = nullptr;

        FIELD()
        ColorComponentInput* inputR = nullptr;

        FIELD()
        ColorComponentInput* inputG = nullptr;

        FIELD()
        ColorComponentInput* inputB = nullptr;

        FIELD()
        ColorComponentInput* inputA = nullptr;

        FIELD()
        ColorComponentInput* inputH = nullptr;

        FIELD()
        ColorComponentInput* inputS = nullptr;

        FIELD()
        ColorComponentInput* inputV = nullptr;

        FIELD()
        ColorPickerPreview* previewOld = nullptr;

        FIELD()
        ColorPickerPreview* previewNew = nullptr;

        FIELD()
        Vec3 hsv = {};

        FIELD()
        bool enableAlpha = true;

    };
    
} // namespace CE::Editor

#include "ColorPickerTool.rtti.h"