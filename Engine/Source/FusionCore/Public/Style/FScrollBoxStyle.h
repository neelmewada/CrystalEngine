#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FScrollBoxStyle : public FStyle
    {
        CE_CLASS(FScrollBoxStyle, FStyle)
    public:

        FScrollBoxStyle();

        virtual ~FScrollBoxStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

        FIELD()
		Color background = Color::RGBA(50, 50, 50);

		FIELD()
		Color color = Color::RGBA(255, 255, 255, 100);

		FIELD()
		Color hoverColor = Color::RGBA(255, 255, 255, 140);

    };
    
} // namespace CE

#include "FScrollBoxStyle.rtti.h"
