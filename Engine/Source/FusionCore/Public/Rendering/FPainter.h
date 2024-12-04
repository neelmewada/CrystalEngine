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

        const FFont& GetCurrentFont();

        void PushOpacity(f32 opacity);
        void PopOpacity();

        void SetItemTransform(const Matrix4x4& transform);

        void PushChildCoordinateSpace(const Matrix4x4& coordinateTransform);
        Matrix4x4 GetTopCoordinateSpace();
        void PopChildCoordinateSpace();

        void PushClipRect(const Matrix4x4& clipTransform, Vec2 rectSize);
        void PopClipRect();

        Vec2 CalculateTextSize(const String& text, const FFont& font, f32 width = 0, FWordWrap wordWrap = FWordWrap::Normal);

        Vec2 CalculateCharacterOffsets(Array<Rect>& outRects, const String& text, const FFont& font, f32 width = 0, FWordWrap wordWrap = FWordWrap::Normal);

        FFontMetrics GetFontMetrics(const FFont& font);

        FusionRenderer2* GetRenderer() const { return renderer2; }

        // - Draw API -

        bool DrawShape(const Rect& rect, const FShape& shape);

        void DrawFrameBuffer(const Rect& rect, const StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount>& frames);

        bool DrawRect(const Rect& rect);

        bool DrawCircle(const Rect& rect);

        bool DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius);

        bool DrawLine(const Vec2& startPos, const Vec2& endPos);

        Vec2 DrawText(const String& text, Vec2 pos, Vec2 size = Vec2(), FWordWrap wordWrap = FWordWrap::Normal);

        bool IsCulled(Vec2 pos, Vec2 quadSize);

    private:

        FusionRenderer* renderer = nullptr;
        FusionRenderer2* renderer2 = nullptr;

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FPainter.rtti.h"