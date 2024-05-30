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

    protected:

        void Construct() override;

        CGradient gradient{};

    };
    
}

#include "ColorComponentInput.rtti.h"
