#include "CoreRHI.h"

namespace CE::RHI
{
    FrameGraphVariable::FrameGraphVariable(bool boolValue)
    {
        type = FrameGraphVariableType::Bool;
        this->boolValue = boolValue;
    }

    FrameGraphVariable::FrameGraphVariable(int intValue)
    {
        type = FrameGraphVariableType::Int;
        this->intValue = intValue;
    }

    FrameGraphVariable::FrameGraphVariable(f32 floatValue)
    {
        type = FrameGraphVariableType::Float;
        this->floatValue = floatValue;
    }

    FrameGraphVariable::~FrameGraphVariable()
    {

    }

    f32 FrameGraphVariable::GetNumberValue() const
    {
        switch (type)
        {
        case FrameGraphVariableType::Bool:
            return boolValue ? 1.0f : 0.0f;
        case FrameGraphVariableType::Int:
            return intValue;
        case FrameGraphVariableType::Float:
            return floatValue;
        }
        return 0;
    }

    bool ExecuteCondition::Compare(const FrameGraphVariable& value) const
    {
        f32 inValue = value.GetNumberValue();
        f32 compValue = comparisonValue.GetNumberValue();

        switch (comparisonOperation)
        {
        case FrameGraphVariableComparison::LessThan:
            return inValue < compValue;
        case FrameGraphVariableComparison::LessThanOrEqual:
            return inValue <= compValue;
        case FrameGraphVariableComparison::Equal:
            return inValue == compValue;
        case FrameGraphVariableComparison::GreaterThan:
            return inValue > compValue;
        case FrameGraphVariableComparison::GreaterThanOrEqual:
            return inValue >= compValue;
        }

        return true;
    }

} // namespace CE::RHI
