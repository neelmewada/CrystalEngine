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

        void SetPen(const CPen& pen);
        void SetBrush(const CBrush& brush);

        void SetRotation(f32 rotation);

        void DrawRect(const Rect& rect);
        void DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius);
        void DrawRoundedX(const Rect& rect);

        void SetFont(const CFont& font);

        Vec2 CalculateTextSize(const String& text, f32 width = 0);

        void DrawText(const String& text, const Vec2& position);
        void DrawText(const String& text, const Rect& rect);

        void PushChildCoordinateSpace(Vec2 newOrigin);
        void PopChildCoordinateSpace();

        void PushChildClipRect(const Rect& clipRect);
        void PopChildClipRect();

    private:

        Array<Vec2> coordinateSpaceStack{};

        Vec2 GetOrigin() const
        {
            if (coordinateSpaceStack.IsEmpty())
                return Vec2(0, 0);
	        return coordinateSpaceStack.Top();
        }

        Renderer2D* renderer = nullptr;
        int numFontsPushed = 0;

        friend class CWidget;
        friend class CWindow;
    };
    
} // namespace CE::Widgets

#include "CPainter.rtti.h"