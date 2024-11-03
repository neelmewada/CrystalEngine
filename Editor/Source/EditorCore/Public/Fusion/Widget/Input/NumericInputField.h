#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API NumericInputField : public FTextInput
    {
        CE_CLASS(NumericInputField, FTextInput)
    protected:

        NumericInputField();

        void Construct() override;

    public: // - Public API -

        

    protected: // - Internal -

        void OnFinishEdit() override;

        void OnPaint(FPainter* painter) override;

        TypeId numericType = 0;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(f32, RangeMin);
        FUSION_PROPERTY(f32, RangeMax);

        FUSION_PROPERTY(bool, ColorTagVisible);
        FUSION_PROPERTY(Color, ColorTag);

        template<typename T> requires TIsNumericType<T>::Value
        Self& NumericType()
        {
            numericType = TYPEID(T);
            Validator(FNumericInputValidator<T>);
            return *this;
        }

        Self& NumericType(TypeId type);

        FUSION_WIDGET;
    };
    
}

#include "NumericInputField.rtti.h"
