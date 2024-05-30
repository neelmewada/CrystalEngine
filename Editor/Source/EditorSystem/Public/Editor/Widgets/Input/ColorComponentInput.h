#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API ColorComponentInput : public NumericFieldInput
    {
        CE_CLASS(ColorComponentInput, NumericFieldInput)
    public:

        ColorComponentInput();

        CGradient& GetGradient() { return gradient; }

        void SetGradient(const CGradient& gradient) { this->gradient = gradient; }

        void ShowTransparency(bool show);

    protected:

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        FIELD()
        CGradient gradient{};

        FIELD()
        bool showTransparency = false;

    };
    
}

#include "ColorComponentInput.rtti.h"
