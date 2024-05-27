#include "EditorSystem.h"

namespace CE::Editor
{

    NumericFieldInput::NumericFieldInput()
    {

    }

    NumericFieldInput::~NumericFieldInput()
    {
        
    }

    void NumericFieldInput::SetFieldType(NumericFieldType fieldType)
    {
        this->fieldType = fieldType;

        switch (fieldType)
        {
        case NumericFieldType::Float32:
        case NumericFieldType::Float64:
            SetInputValidator(CFloatInputValidator);
            break;
        case NumericFieldType::Uint8:
        case NumericFieldType::Uint16:
        case NumericFieldType::Uint32:
        case NumericFieldType::Uint64:
            SetInputValidator(CUnsignedIntegerInputValidator);
            break;
        case NumericFieldType::Int8:
        case NumericFieldType::Int16:
        case NumericFieldType::Int32:
        case NumericFieldType::Int64:
            SetInputValidator(CIntegerInputValidator);
            break;
        }
    }

    void NumericFieldInput::SetPrecision(u32 precision)
    {
        this->precision = precision;
    }

    void NumericFieldInput::OnValidateText()
    {
	    Super::OnValidateText();

        if (fieldType == NumericFieldType::Float32 || fieldType == NumericFieldType::Float64)
        {
            f64 number = 0;
            if (String::TryParse(text, number))
            {
                text = String::Format("{:.{}f}", number, precision);
            }
        }
    }

    void NumericFieldInput::Construct()
    {
        Super::Construct();

        SetSelectAllOnEdit(true);
    }
    
}

