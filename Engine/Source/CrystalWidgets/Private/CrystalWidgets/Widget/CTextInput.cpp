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
        if (value.IsEmptyOrWhiteSpace())
            return Vec2();

        Vec2 base = Super::CalculateIntrinsicSize(width, height);

        Renderer2D* renderer = nullptr;
        CWindow* owner = ownerWindow;
        while (owner != nullptr)
        {
            if (owner->ownerWindow == nullptr)
            {
                renderer = owner->renderer;
                break;
            }
            owner = owner->ownerWindow;
        }

        if (!renderer)
            return base;

        Name fontName = computedStyle.properties[CStylePropertyType::FontName].string;

        f32 fontSize = 14;
        if (computedStyle.properties.KeyExists(CStylePropertyType::FontSize))
            fontSize = computedStyle.properties[CStylePropertyType::FontSize].single;

        if (fontSize < 8)
            fontSize = 8;

        return renderer->CalculateTextSize(value, fontSize, fontName, 0);
    }

    void CTextInput::SetValue(const String& value)
    {
        this->value = value;

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

    void CTextInput::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = (CMouseEvent*)event;

            if (mouseEvent->type == CEventType::MouseEnter)
            {
                
            }
            else if (mouseEvent->type == CEventType::MouseLeave)
            {
                
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

        String display = value;


        if (display.IsEmpty())
            return;

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

        Vec2 textSize = painter->CalculateTextSize(value, isMultiline ? rect.GetSize().width : 0);
        Rect textRect = rect.Translate(Vec2(padding.left, rect.GetSize().height / 2 - textSize.height / 2));
        textRect.max -= Vec2(padding.left + padding.right, rect.GetSize().height / 2 - textSize.height / 2);

        painter->PushClipRect(textRect);
        painter->DrawText(display, textRect.min);
        painter->PopClipRect();
    }

} // namespace CE::Widgets
