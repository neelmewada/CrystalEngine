#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API ColorInput : public CWidget
    {
        CE_CLASS(ColorInput, CWidget)
    public:

        ColorInput();

        virtual ~ColorInput();

        const Color& GetValue() const { return value; }

        void SetValue(const Color& value);

        bool HasAlpha() const { return hasAlpha; }

        void SetHasAlpha(bool set);

        // - Events -

        FIELD()
        CColorEvent onColorSelected;

    protected:

        void OnBeforeDestroy() override;

        void Construct() override;

        void OnPaintEarly(CPaintEvent* paintEvent) override;

        void HandleEvent(CEvent* event) override;

        FUNCTION()
        void OnColorSelected(Color newColor);

        FUNCTION()
        void OnColorPickerToolClosed(ColorPickerTool* colorPicker);

        FIELD()
        Color value{};
        
        FIELD()
        b8 hasAlpha = true;

        FIELD()
        ColorPickerTool* colorPicker = nullptr;

        friend class ColorFieldEditor;
    };
    
} // namespace CE::Editor

#include "ColorInput.rtti.h"