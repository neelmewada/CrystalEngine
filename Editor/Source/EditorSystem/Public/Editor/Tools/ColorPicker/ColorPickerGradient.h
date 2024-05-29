#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API ColorPickerGradient : public CWidget
    {
        CE_CLASS(ColorPickerGradient, CWidget)
    public:

        ColorPickerGradient();

        virtual ~ColorPickerGradient();

        const CGradient& GetGradient() const
        {
            return gradient;
        }

        CGradient& GetGradient()
        {
            return gradient;
        }

        void SetGradient(const CGradient& gradient);

    protected:

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        FIELD()
        CGradient gradient{};

        friend class ColorPickerTool;
    };
    
}

#include "ColorPickerGradient.rtti.h"
