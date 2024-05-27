#pragma once

namespace CE::Editor
{
    ENUM()
    enum class NumericFieldType
    {
        Float32, Float64,
        Uint8, Int8,
        Uint16, Int16,
        Uint32, Int32,
        Uint64, Int64,
    };
    ENUM_CLASS(NumericFieldType);

    CLASS()
    class EDITORSYSTEM_API NumericFieldInput : public CTextInput
    {
        CE_CLASS(NumericFieldInput, CTextInput)
    public:

        NumericFieldInput();

        virtual ~NumericFieldInput();

        void SetFieldType(NumericFieldType fieldType);

        void SetPrecision(u32 precision);

    protected:

        void OnValidateText() override;

        void Construct() override;

        FIELD()
        NumericFieldType fieldType = NumericFieldType::Float32;

        FIELD()
        u32 precision = 6;

    };
    
}

#include "NumericFieldInput.rtti.h"
