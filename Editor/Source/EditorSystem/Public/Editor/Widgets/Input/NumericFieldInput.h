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

        bool IsSignedInt() const
        {
            return fieldType == NumericFieldType::Int8 || fieldType == NumericFieldType::Int16 ||
                fieldType == NumericFieldType::Int32 || fieldType == NumericFieldType::Int64;
        }

        bool IsUnsignedInt() const
        {
            return fieldType == NumericFieldType::Uint8 || fieldType == NumericFieldType::Uint16 ||
                fieldType == NumericFieldType::Uint32 || fieldType == NumericFieldType::Uint64;
        }

    protected:

        bool OnAfterComputeStyle() override;

        void OnValidateText() override;

        void Construct() override;

        void HandleEvent(CEvent* event) override;

        FIELD()
        NumericFieldType fieldType = NumericFieldType::Float32;

        FIELD()
        u32 precision = 6;

        FIELD()
        f32 floatSensitivity = 0.1f;

        FIELD()
        f32 integerSensitivity = 0.5f;

        Vec2 startMousePos = {};

        f64 floatValue = 0;

        s64 signedValue = 0;

        u64 unsignedValue = 0;
    };
    
}

#include "NumericFieldInput.rtti.h"
