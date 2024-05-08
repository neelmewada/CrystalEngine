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

    Vec2 CPainter::CalculateTextOffsets(Array<Rect>& outOffsets, const String& text, f32 width)
    {
        return renderer->CalculateTextOffsets(outOffsets, text, width);
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
        if (cornerRadius == Vec4())
            renderer->DrawRect(rect.GetSize());
        else
			renderer->DrawRoundedRect(rect.GetSize(), cornerRadius);
    }

    void CPainter::DrawRoundedX(const Rect& rect)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawRoundedX(rect.GetSize());
    }

    void CPainter::DrawTexture(const Rect& rect, RPI::Texture* texture)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawTexture(texture, rect.GetSize());
    }

    void CPainter::DrawFrameBuffer(const Rect& rect,
	    const StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount>& frames)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawFrameBuffer(frames, rect.GetSize());
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

    void CPainter::PushClipRect(const Rect& clipRect)
    {
        Rect rect = Rect::FromSize(GetOrigin() + clipRect.min, clipRect.GetSize());
        renderer->PushClipRect(rect);
    }

    void CPainter::PopClipRect()
    {
        renderer->PopClipRect();
    }

    bool CPainter::ClipRectExists()
    {
        return renderer->ClipRectExists();
    }

    Rect CPainter::GetLastClipRect()
    {
        return renderer->GetLastClipRect();
    }

} // namespace CE::Widgets
