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
        this->pen = pen;
        renderer->SetBorderThickness(pen.width);
        renderer->SetOutlineColor(pen.color);
    }

    void CPainter::SetBrush(const CBrush& brush)
    {
        this->brush = brush;
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

    void CPainter::SetRotation(f32 degrees)
    {
        renderer->SetRotation(degrees);
    }

    void CPainter::DrawCircle(const Rect& rect)
    {
        Rect windowSpaceRect = Rect::FromSize(GetOrigin() + rect.min, rect.GetSize());
        if (renderer->ClipRectExists())
        {
            Rect clipRect = renderer->GetLastClipRect();
            if (!clipRect.Overlaps(windowSpaceRect))
                return;
        }

        renderer->SetCursor(windowSpaceRect.min);
        renderer->DrawCircle(rect.GetSize());
    }

    void CPainter::DrawLine(Vec2 from, Vec2 to)
    {
        Vec2 center = (from + to) / 2.0f;
        float rotation = atan2(to.y - from.y, to.x - from.x);
        float width = sqrt((to.x - from.x) * (to.x - from.x) + (to.y - from.y) * (to.y - from.y));
        float height = pen.width;  // You define the thickness of the line
        if (height < 0.001f)
            return;

        f32 origRotation = renderer->GetRotation();

        SetRotation(rotation * RAD_TO_DEG);

        Rect rect = Rect::FromSize(from.x, center.y, width, height);

        if (pen.style == CPenStyle::SolidLine)
        {
	        DrawRect(rect);
        }
        else
        {
            DrawDashedLine(rect);
        }

        renderer->SetRotation(origRotation);
    }

    void CPainter::DrawDashedLine(const Rect& rect)
    {
        Rect windowSpaceRect = Rect::FromSize(GetOrigin() + rect.min, rect.GetSize());
        if (renderer->ClipRectExists())
        {
            Rect clipRect = renderer->GetLastClipRect();
            if (!clipRect.Overlaps(windowSpaceRect))
                return;
        }

        renderer->SetCursor(windowSpaceRect.min);
        renderer->DrawDashedLine(rect.GetSize(), pen.dashLength);
    }

    void CPainter::DrawRect(const Rect& rect)
    {
        Rect windowSpaceRect = Rect::FromSize(GetOrigin() + rect.min, rect.GetSize());
        if (renderer->ClipRectExists())
        {
            Rect clipRect = renderer->GetLastClipRect();
            if (!clipRect.Overlaps(windowSpaceRect))
                return;
        }
        
        renderer->SetCursor(windowSpaceRect.min);
        renderer->DrawRect(rect.GetSize());
    }

    void CPainter::DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius)
    {
        Rect windowSpaceRect = Rect::FromSize(GetOrigin() + rect.min, rect.GetSize());
        if (renderer->ClipRectExists())
        {
            Rect clipRect = renderer->GetLastClipRect();
            if (!clipRect.Overlaps(windowSpaceRect))
                return;
        }

        renderer->SetCursor(windowSpaceRect.min);
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

    void CPainter::DrawTriangle(const Rect& rect)
    {
        renderer->SetCursor(GetOrigin() + rect.min);
        renderer->DrawTriangle(rect.GetSize());
    }

    void CPainter::DrawText(const String& text, const Vec2& position)
    {
        if (renderer->ClipRectExists())
        {
            Vec2 textSize = renderer->CalculateTextSize(text);
            Rect windowSpaceRect = Rect::FromSize(GetOrigin() + position, textSize);
            Rect clipRect = renderer->GetLastClipRect();

            if (!windowSpaceRect.Overlaps(clipRect))
                return;
        }

        renderer->SetCursor(GetOrigin() + position);
        renderer->DrawText(text);
    }

    void CPainter::DrawText(const String& text, const Rect& rect)
    {
        Rect windowSpaceRect = Rect::FromSize(GetOrigin() + rect.min, rect.GetSize());
        if (renderer->ClipRectExists())
        {
            Rect clipRect = renderer->GetLastClipRect();

            if (!windowSpaceRect.Overlaps(clipRect))
                return;
        }

        renderer->SetCursor(windowSpaceRect.min);
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
