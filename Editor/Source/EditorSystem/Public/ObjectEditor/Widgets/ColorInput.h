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

    protected:

        void Construct() override;

        void OnPaintEarly(CPaintEvent* paintEvent) override;

        FIELD()
        Color value{};
        
        FIELD()
        b8 hasAlpha = true;

        friend class ColorFieldEditor;
    };
    
} // namespace CE::Editor

#include "ColorInput.rtti.h"