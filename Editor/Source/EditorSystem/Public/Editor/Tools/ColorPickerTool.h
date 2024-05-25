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

    private:

        void Construct() override;

        FIELD()
        Color original{};

        FIELD()
        Color value{};

        FIELD()
        bool enableAlpha = true;

    };
    
} // namespace CE::Editor

#include "ColorPickerTool.rtti.h"