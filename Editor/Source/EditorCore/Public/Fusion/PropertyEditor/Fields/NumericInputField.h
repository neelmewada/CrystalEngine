#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API NumericInputField : public EditorField
    {
        CE_CLASS(NumericInputField, EditorField)
    protected:

        NumericInputField();

        void Construct() override;

    public: // - Public API -

        

    protected: // - Internal -

        void OnPaint(FPainter* painter) override;

        FUNCTION()
        void OnFinishEdit(FTextInput* field);

        TypeId numericType = 0;
        FTextInput* input = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(f32, RangeMin);
        FUSION_PROPERTY(f32, RangeMax);

        FUSION_PROPERTY(bool, ColorTagVisible);
        FUSION_PROPERTY(Color, ColorTag);

        template<typename T> requires TIsNumericType<T>::Value
        Self& NumericType()
        {
            numericType = TYPEID(T);
            input->Validator(FNumericInputValidator<T>);
            return *this;
        }

        Self& NumericType(TypeId type);

        FUSION_WIDGET;
    };
    
}

#include "NumericInputField.rtti.h"
