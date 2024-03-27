#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CLabel::CLabel()
    {
        receiveDragEvents = receiveMouseEvents = false;
        allowVerticalScroll = allowHorizontalScroll = false;
    }

    CLabel::~CLabel()
    {

    }

    Vec2 CLabel::CalculateIntrinsicSize(f32 width, f32 height)
    {
        if (text.IsEmpty())
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

        return renderer->CalculateTextSize(text, fontSize, fontName, 0);
    }

    void CLabel::SetText(const String& text)
    {
        this->text = text;
    }

    const String& CLabel::GetText() const
    {
        return text;
    }

    void CLabel::OnPaint(CPaintEvent* paintEvent)
    {
        Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Name fontName = computedStyle.properties[CStylePropertyType::FontName].string;

        f32 fontSize = 14;
        if (computedStyle.properties.KeyExists(CStylePropertyType::FontSize))
            fontSize = computedStyle.properties[CStylePropertyType::FontSize].single;

        CFont font = CFont(fontName, (u32)fontSize, false);
        CPen pen = CPen(Color::White());

        painter->SetFont(font);

        Rect rect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());
        rect = Rect::FromSize(0, 0, 100, 20);

        painter->DrawText(text, rect);
    }

    
} // namespace CE::Widgets
