#pragma once

namespace CE::RHI
{
    enum class FrameGraphVariableType
    {
        None = 0,
        Bool,
        Int,
        Float
    };
    ENUM_CLASS(FrameGraphVariableType);

    enum class FrameGraphVariableComparison
    {
        None = 0,
        LessThan,
        LessThanOrEqual,
        Equal,
        GreaterThan,
        GreaterThanOrEqual
    };
    ENUM_CLASS(FrameGraphVariableComparison);

    class CORERHI_API FrameGraphVariable
    {
    public:

        FrameGraphVariable() : intValue(0) {}

        FrameGraphVariable(bool boolValue);
        FrameGraphVariable(int intValue);
        FrameGraphVariable(f32 floatValue);

        ~FrameGraphVariable();

        inline FrameGraphVariable(const FrameGraphVariable& copy)
        {
            memcpy(this, &copy, sizeof(FrameGraphVariable));
        }

        inline FrameGraphVariable& operator=(const FrameGraphVariable& copy)
        {
            memcpy(this, &copy, sizeof(FrameGraphVariable));
            return *this;
        }

        inline FrameGraphVariableType GetType() const { return type; }

        inline bool GetBoolValue() const { return boolValue; }
        inline int GetIntValue() const { return intValue; }
        inline f32 GetFloatValue() const { return floatValue; }

        f32 GetNumberValue() const;

    private:

        FrameGraphVariableType type{};

        union
        {
            bool boolValue;
            int intValue;
            f32 floatValue;
        };

    };

    struct CORERHI_API ExecuteCondition
    {
        Name variableName = {};
        FrameGraphVariableComparison comparisonOperation = FrameGraphVariableComparison::None;
        FrameGraphVariable comparisonValue = {};
        bool shouldClear = false;

        bool Compare(const FrameGraphVariable& value) const;

    };
    
} // namespace CE::RHI
