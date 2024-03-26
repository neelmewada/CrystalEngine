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

    void CPainter::SetFont(const CFont& font)
    {
        renderer->PushFont(font.family, font.size, font.bold);
        numFontsPushed++;
    }

    Vec2 CPainter::CalculateTextSize(const String& text, f32 width)
    {
        return renderer->CalculateTextSize(text, width);
    }

    void CPainter::SetRotation(f32 rotation)
    {
        renderer->SetRotation(rotation);
    }

    void CPainter::DrawRect(const Rect& rect)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawRect(rect.GetSize());
    }

    void CPainter::DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawRoundedRect(rect.GetSize(), cornerRadius);
    }

    void CPainter::DrawRoundedX(const Rect& rect)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawRoundedX(rect.GetSize());
    }

    void CPainter::DrawText(const String& text, const Vec2& position)
    {
        renderer->SetCursor(GetOrigin() + position);
        renderer->DrawText(text);
    }

    void CPainter::DrawText(const String& text, const Rect& rect)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawText(text, rect.GetSize());
    }

    void CPainter::PushChildCoordinateSpace(Vec2 newOrigin)
    {
        if (coordinateSpaceStack.IsEmpty())
        {
	        coordinateSpaceStack.Push(newOrigin);
        }
        else
        {
            coordinateSpaceStack.Push(coordinateSpaceStack.Top() + newOrigin);
        }
    }

    void CPainter::PopChildCoordinateSpace()
    {
        coordinateSpaceStack.Pop();
    }

    void CPainter::PushChildClipRect(const Rect& clipRect)
    {
        Rect rect = Rect::FromSize(GetOrigin() + clipRect.min, clipRect.GetSize());
        renderer->PushClipRect(rect);
    }

    void CPainter::PopChildClipRect()
    {
        renderer->PopClipRect();
    }

} // namespace CE::Widgets
