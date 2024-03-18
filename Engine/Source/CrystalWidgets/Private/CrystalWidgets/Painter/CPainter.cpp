#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CPainter::CPainter()
    {

    }

    CPainter::~CPainter()
    {

    }

    void CPainter::Reset()
    {
        renderer->ResetToDefaults();

        for (int i = 0; i < numFontsPushed; i++)
        {
            renderer->PopFont();
        }

        numFontsPushed = 0;
    }

    void CPainter::SetPen(const CPen& pen)
    {
        renderer->SetBorderThickness(pen.width);
        renderer->SetOutlineColor(pen.color);
    }

    void CPainter::SetBrush(const CBrush& brush)
    {
        renderer->SetFillColor(brush.color);
    }

    void CPainter::SetRotation(f32 rotation)
    {
        renderer->SetRotation(rotation);
    }

    void CPainter::DrawRect(const Rect& rect)
    {
        renderer->SetCursor(rect.min);
        renderer->DrawRect(rect.GetSize());
    }

    void CPainter::DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius)
    {
        renderer->SetCursor(rect.min);
        renderer->DrawRoundedRect(rect.GetSize(), cornerRadius);
    }

    void CPainter::SetFont(const CFont& font)
    {
        renderer->PushFont(font.family, font.size, font.bold);
        numFontsPushed++;
    }

    void CPainter::DrawText(const String& text, const Vec2& position)
    {
        renderer->SetCursor(position);
        renderer->DrawText(text);
    }

    void CPainter::DrawText(const String& text, const Rect& rect)
    {
        renderer->SetCursor(rect.min);
        renderer->DrawText(text, rect.GetSize());
    }

} // namespace CE::Widgets
