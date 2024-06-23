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
        void SetFont(const FFont& font);

        void PushOpacity(f32 opacity);
        void PopOpacity();

        void SetItemTransform(const Matrix4x4& transform);

        void PushChildCoordinateSpace(const Matrix4x4& coordinateTransform);
        const Matrix4x4& GetTopCoordinateSpace();
        void PopChildCoordinateSpace();

        void PushClipShape(const Matrix4x4& clipTransform, Vec2 rectSize, const FShape& clipShape = FRectangle());
        void PopClipShape();

        Vec2 CalculateTextSize(const String& text, const FFont& font, f32 width = 0, FWordWrap wordWrap = FWordWrap::Normal);

        Vec2 CalculateCharacterOffsets(Array<Rect>& outRects, const String& text, const FFont& font, f32 width = 0, FWordWrap wordWrap = FWordWrap::Normal);

        FFontMetrics GetFontMetrics(const FFont& font);

        // - Draw API -

        void DrawShape(const Rect& rect, const FShape& shape);

        void DrawRect(const Rect& rect);

        void DrawCircle(const Rect& rect);

        void DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius);

        void DrawLine(const Vec2& startPos, const Vec2& endPos);

        Vec2 DrawText(const String& text, Vec2 pos, Vec2 size = Vec2(), FWordWrap wordWrap = FWordWrap::Normal);

    private:

        FusionRenderer* renderer = nullptr;

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FPainter.rtti.h"