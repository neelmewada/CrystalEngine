#pragma once

namespace CE
{

    struct FUSIONCORE_API FShape final
    {
    public:

        FShape() = default;

        FShape(FShapeType shapeType);

        static FShape Rect();
        static FShape Circle();

        static FShape RoundedRect(const Vec4& cornerRadius);

        FShapeType GetShapeType() const { return shapeType; }

        const Vec4& GetCornerRadius() const { return cornerRadius; }

        bool operator==(const FShape& rhs) const;

        bool operator!=(const FShape& rhs) const
        {
            return !operator==(rhs);
        }

    private:

        FShapeType shapeType = FShapeType::None;
        Vec4 cornerRadius{};

    };

    inline FShape FRectangle() { return FShape::Rect(); }
    inline FShape FCircle() { return FShape::Circle(); }
    inline FShape FRoundedRectangle(const Vec4& cornerRadius) { return FShape::RoundedRect(cornerRadius); }
    inline FShape FRoundedRectangle(f32 cornerRadius) { return FShape::RoundedRect(Vec4(1, 1, 1, 1) * cornerRadius); }
    
} // namespace CE

#include "FShape.rtti.h"