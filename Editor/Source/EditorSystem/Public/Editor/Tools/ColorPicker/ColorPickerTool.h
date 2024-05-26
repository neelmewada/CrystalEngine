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

        void EnableAlpha(bool enable);

        void SetOriginalColor(const Color& original);

        void SetColor(const Color& color);

        // - Signals -

        CE_SIGNAL(OnColorSelected, Color);

        CE_SIGNAL(OnColorPickerClosed, ColorPickerTool*);

    private:

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
        Vec3 hsv = {};

        FIELD()
        bool enableAlpha = true;

    };
    
} // namespace CE::Editor

#include "ColorPickerTool.rtti.h"