#pragma once

namespace CE::Widgets
{
	class CWidget;

    CLASS()
    class CRYSTALWIDGETS_API CPainter final : public Object
    {
        CE_CLASS(CPainter, Object)
    public:

        CPainter();
        virtual ~CPainter();

        void Reset();

        Name GetDefaultFontName() const { return renderer->GetDefaultFontName(); }

        void SetPen(const CPen& pen);
        void SetBrush(const CBrush& brush);

        void SetRotation(f32 degrees);

        void DrawCircle(const Rect& rect);

        void DrawLine(Vec2 from, Vec2 to);

        void DrawRect(const Rect& rect);
        void DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius);
        void DrawRoundedX(const Rect& rect);

        void DrawTexture(const Rect& rect, RPI::Texture* texture);

        void DrawFrameBuffer(const Rect& rect, const StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount>& frames);

        void DrawTriangle(const Rect& rect);

        void SetFont(const CFont& font);

        Vec2 CalculateTextSize(const String& text, f32 width = 0);

        Vec2 CalculateTextOffsets(Array<Rect>& outOffsets, const String& text, f32 width = 0);

        void DrawText(const String& text, const Vec2& position);
        void DrawText(const String& text, const Rect& rect);

        void PushChildCoordinateSpace(Vec2 newOrigin);
        void PopChildCoordinateSpace();

        void PushClipRect(const Rect& clipRect, const Vec4& cornerRadius = {});
        void PopClipRect();

        bool ClipRectExists();
        Rect GetLastClipRect();

        Vec2 GetCurrentOrigin() const { return GetOrigin(); }

        const CPen& GetPen() const { return pen; }
        const CFont& GetFont() const { return font; }
        const CBrush& GetBrush() const { return brush; }

    private:

        void DrawDashedLine(const Rect& rect);

        Array<Vec2> coordinateSpaceStack{};

        Vec2 GetOrigin() const
        {
            if (coordinateSpaceStack.IsEmpty())
                return Vec2(0, 0);
	        return coordinateSpaceStack.Top();
        }

        Renderer2D* renderer = nullptr;
        int numFontsPushed = 0;

        CPen pen{};
        CBrush brush{};
        CFont font{};

        friend class CWidget;
        friend class CWindow;
        CE_WIDGET_FRIENDS();
    };
    
} // namespace CE::Widgets

#include "CPainter.rtti.h"