#pragma once

namespace CE
{
    class FusionRenderer;

    CLASS()
    class FUSIONCORE_API FPainter final : public Object
    {
        CE_CLASS(FPainter, Object)
    public:

        FPainter();

        // - State API -

        void SetPen(const FPen& pen);
        void SetBrush(const FBrush& brush);

        void PushOpacity(f32 opacity);
        void PopOpacity();

        void SetItemTransform(const Matrix4x4& transform);

        void PushChildCoordinateSpace(const Matrix4x4& coordinateTransform);
        void PopChildCoordinateSpace();

        void PushClipShape(const Matrix4x4& clipTransform, Vec2 rectSize, const FShape& clipShape = FRectangle());
        void PopClipShape();

        // - Draw API -

        void DrawShape(const Rect& rect, const FShape& shape);

        void DrawRect(const Rect& rect);

        void DrawCircle(const Rect& rect);

        void DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius);

    private:

        FusionRenderer* renderer = nullptr;

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FPainter.rtti.h"