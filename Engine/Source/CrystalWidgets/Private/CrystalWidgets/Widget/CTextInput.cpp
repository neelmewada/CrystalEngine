#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CTextInput::CTextInput()
    {
        receiveMouseEvents = true;
        receiveDragEvents = true;
        allowVerticalScroll = allowHorizontalScroll = false;
        interactable = true;

        //clipChildren = true;
    }

    CTextInput::~CTextInput()
    {
	    
    }

    Vec2 CTextInput::CalculateIntrinsicSize(f32 width, f32 height)
    {
        if (text.IsEmpty())
            return Vec2();

        String display = GetDisplayText();

        Vec2 base = Super::CalculateIntrinsicSize(width, height);

        Name fontName = computedStyle.properties[CStylePropertyType::FontName].string;

        f32 fontSize = 14;
        if (computedStyle.properties.KeyExists(CStylePropertyType::FontSize))
            fontSize = computedStyle.properties[CStylePropertyType::FontSize].single;

        if (fontSize < 8)
            fontSize = 8;
        return CalculateTextSize(display, fontSize, fontName, 0);
    }

    void CTextInput::SetText(const String& value)
    {
        this->text = value;

        SetNeedsLayout();
        SetNeedsPaint();
    }

    void CTextInput::SetHint(const String& hint)
    {
        this->hint = hint;

        SetNeedsPaint();
    }

    void CTextInput::SetAsPassword(b8 set)
    {
        if (isPassword == set)
            return;

        isPassword = set;

        SetNeedsLayout();
        SetNeedsPaint();
    }

    String CTextInput::GetDisplayText()
    {
        String display = text;

        if (isPassword)
        {
            display.Clear();
            for (int i = 0; i < text.GetLength(); ++i)
            {
                display.Append('*');
            }
        }

        return display.GetSubstring(charStartOffset);
    }

    void CTextInput::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = (CMouseEvent*)event;
            Vec2 screenMousePos = mouseEvent->mousePos;
            Vec2 localMousePos = ScreenSpaceToLocalPoint(screenMousePos);
            Vec4 padding = GetComputedLayoutPadding();

            Renderer2D* renderer = GetRenderer();
            String display = text;

            Name fontName = computedStyle.properties[CStylePropertyType::FontName].string;

            f32 fontSize = 14;
            if (computedStyle.properties.KeyExists(CStylePropertyType::FontSize))
                fontSize = computedStyle.properties[CStylePropertyType::FontSize].single;

            if (renderer && mouseEvent->type == CEventType::MouseEnter)
            {

            }
            else if (renderer && mouseEvent->type == CEventType::MouseLeave)
            {

            }
            else if (renderer && mouseEvent->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
            {
                Array<Rect> offsets{};
                Vec2 size = renderer->CalculateTextOffsets(offsets, display, fontSize, fontName);

                int selectedIdx = -1;

                for (int i = 0; i < offsets.GetSize(); ++i)
                {
                    Rect localPos = offsets[i].Translate(padding.min);

                    if (localMousePos.x < localPos.left + localPos.GetSize().width * 0.4f)
                    {
                        selectedIdx = i + charStartOffset;
                        CE_LOG(Info, All, "Select at: {}", selectedIdx);
	                    break;
                    }
                }
            }
        }

        Super::HandleEvent(event);
    }

    Vec2 CTextInput::GetComputedLayoutSize()
    {
        Vec2 size = Super::GetComputedLayoutSize();
        return size;
    }

    void CTextInput::OnPaint(CPaintEvent* paintEvent)
    {
        Super::OnPaint(paintEvent);

        String display = GetDisplayText();
        
        CPainter* painter = paintEvent->painter;

        Name fontName = computedStyle.properties[CStylePropertyType::FontName].string;

        f32 fontSize = 14;
        if (computedStyle.properties.KeyExists(CStylePropertyType::FontSize))
            fontSize = computedStyle.properties[CStylePropertyType::FontSize].single;

        Color color = Color::White();

        if (computedStyle.properties.KeyExists(CStylePropertyType::Foreground))
            color = computedStyle.properties[CStylePropertyType::Foreground].color;

        CFont font = CFont(fontName, (u32)fontSize, false);
        CPen pen = CPen(color);
        
        painter->SetFont(font);
        painter->SetPen(pen);

        Rect rect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());
        Vec4 padding = GetComputedLayoutPadding();

        Vec2 textSize = painter->CalculateTextSize(display, isMultiline ? rect.GetSize().width : 0);
        Rect textRect = rect.Translate(Vec2(padding.left, rect.GetSize().height / 2 - textSize.height / 2));
        textRect.max -= Vec2(padding.left + padding.right, rect.GetSize().height / 2 - textSize.height / 2);
        
        painter->PushClipRect(textRect);
        painter->DrawText(display, textRect.min);
        painter->PopClipRect();
    }

} // namespace CE::Widgets
