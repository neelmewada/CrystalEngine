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

        void SetFont(const CFont& font);

        void DrawText(const String& text, const Vec2& position);
        void DrawText(const String& text, const Rect& rect);

    private:

        Renderer2D* renderer = nullptr;
        int numFontsPushed = 0;

        friend class CWidget;
        friend class CWindow;
    };
    
} // namespace CE::Widgets

#include "CPainter.rtti.h"