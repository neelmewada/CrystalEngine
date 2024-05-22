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

    protected:

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

    };
    
} // namespace CE::Editor

#include "ColorInput.rtti.h"