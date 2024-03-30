#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CTextInput::CTextInput()
    {
        receiveMouseEvents = true;
        receiveDragEvents = true;
        receiveKeyEvents = true;
        allowVerticalScroll = allowHorizontalScroll = false;
        interactable = true;

        timer = CreateDefaultSubobject<CTimer>("Timer");
        if (!IsDefaultInstance())
        {
            Bind(timer, MEMBER_FUNCTION(CTimer, OnTimeOut),
                this, MEMBER_FUNCTION(Self, OnTimerTick));
        }
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

    void CTextInput::OnTimerTick()
    {
        cursorState = !cursorState;
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

        return display;
    }

    void CTextInput::OnFocusGained()
    {
        Super::OnFocusGained();
    }

    void CTextInput::OnFocusLost()
    {
        Super::OnFocusLost();
    }

    void CTextInput::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = (CMouseEvent*)event;
            Vec2 screenMousePos = mouseEvent->mousePos;
            Vec2 mouseDelta = mouseEvent->mousePos - mouseEvent->prevMousePos;
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

                if (mouseEvent->isDoubleClick)
                {
                    SelectAll();
                }

                for (int i = 0; i < offsets.GetSize(); ++i)
                {
                    Rect localPos = offsets[i].Translate(padding.min - Vec2(textScrollOffset, 0));

                    if (localMousePos.x < localPos.left + localPos.GetSize().width * 0.4f)
                    {
                        selectedIdx = i;
                        cursorPos = selectedIdx;
	                    break;
                    }
                }

                if (selectedIdx != -1)
                {
                    timer->Reset();
                    timer->Start(cursorBlinkMillis);
                    cursorState = true;
                    SetNeedsPaint();
                }
            }
            else if (renderer && mouseEvent->type == CEventType::DragBegin && mouseEvent->button == MouseButton::Left && IsEditable() &&
                cursorPos >= 0 && cursorPos < display.GetLength())
            {
                CDragEvent* dragEvent = static_cast<CDragEvent*>(mouseEvent);

                dragEvent->Consume(this);

                dragEvent->draggedWidget = this;
                selectionRange.min = cursorPos;
                selectionRange.max = cursorPos - 1;
                selectionDistance = 0;
            }
            else if (renderer && mouseEvent->type == CEventType::DragMove && mouseEvent->button == MouseButton::Left && IsEditable() && !selectAll)
            {
                CDragEvent* dragEvent = static_cast<CDragEvent*>(mouseEvent);

                dragEvent->Consume(this);

                Rect contentRect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());
                contentRect.min += padding.min;
                contentRect.max -= padding.max;

                Array<Rect> offsets{};
                Vec2 size = renderer->CalculateTextOffsets(offsets, display, fontSize, fontName);

                selectionDistance += mouseDelta.x;
                int selectionIndex = -1;

                selectionRange.min = cursorPos;
                selectionRange.max = -1;

                RangeInt range = RangeInt(-1, -1);
                f32 prevCenterPoint = -NumericLimits<f32>::Infinity();

                for (int i = 0; i < offsets.GetSize(); ++i)
                {
                    Rect characterRect = offsets[i].Translate(padding.min - Vec2(textScrollOffset, 0));
                    Rect cursorCharacterRect = offsets[cursorPos].Translate(padding.min - Vec2(textScrollOffset, 0));
                    f32 centerPoint = (characterRect.min.x + characterRect.max.x) / 2.0f;

                    if (selectionDistance > 0)
                    {
                        if (cursorCharacterRect.min.x + selectionDistance > (characterRect.min.x + characterRect.max.x) * 0.5f)
                        {
                            selectionIndex = i;
                            range.max = i;
                        }
                    }
                    else if (selectionDistance < 0)
                    {
	                    if (cursorCharacterRect.min.x + selectionDistance > prevCenterPoint)
	                    {
                            selectionIndex = i;
                            range.min = i;
	                    }
                    }

                    prevCenterPoint = centerPoint;
                }

                if (selectionDistance > 0 && range.min == -1)
                {
	                range.min = cursorPos;

                    Rect characterRect = offsets[range.max].Translate(padding.min - Vec2(textScrollOffset, 0));
                    Rect lastCharacterRect = offsets.Top().Translate(padding.min);

                    if (characterRect.min.x + characterRect.GetSize().width > contentRect.max.x)
                    {
                        textScrollOffset += mouseDelta.x;
                        textScrollOffset = Math::Min(textScrollOffset, lastCharacterRect.min.x + lastCharacterRect.GetSize().width - contentRect.GetSize().width);
                    }
                }
                if (selectionDistance < 0 && range.max == -1)
                {
	                range.max = cursorPos - 1;

                    Rect characterRect = offsets[range.min].Translate(padding.min - Vec2(textScrollOffset, 0));

                    if (characterRect.min.x + characterRect.GetSize().width < contentRect.min.x)
                    {
                        textScrollOffset += mouseDelta.x;
                        textScrollOffset = Math::Max(textScrollOffset, 0.0f);
                    }
                }

                selectionRange = range;

                SetNeedsPaint();
            }
        }
        else if (event->type == CEventType::FocusChanged)
        {
            CFocusEvent* focusEvent = static_cast<CFocusEvent*>(event);

            if (focusEvent->LostFocus())
            {
                timer->Stop();
                cursorState = false;
                SetNeedsPaint();
            }
        }
        else if (event->type == CEventType::KeyHeld || event->type == CEventType::KeyPress)
        {
            CKeyEvent* keyEvent = static_cast<CKeyEvent*>(event);

        	bool shiftPressed = EnumHasAnyFlags(keyEvent->modifier, KeyModifier::LShift | KeyModifier::RShift);
            bool capslock = EnumHasAnyFlags(keyEvent->modifier, KeyModifier::Caps);

            bool isUpperCase = shiftPressed != capslock;

            char c = 0;

            // A valid text character
            if ((int)keyEvent->key >= (int)KeyCode::Space && (int)keyEvent->key <= (int)KeyCode::Z)
            {
                c = (char)keyEvent->key;

                if (isUpperCase && c >= 'a' && c <= 'z')
                {
                    c += ('A' - 'a');
                }

                CE_LOG(Info, All, "Append: {}", c);
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
        Color textSelectionColor = Color::RGBA8(0, 112, 224);

        if (computedStyle.properties.KeyExists(CStylePropertyType::Foreground))
            color = computedStyle.properties[CStylePropertyType::Foreground].color;

        CFont font = CFont(fontName, (u32)fontSize, false);
        CPen pen = CPen(color);
        CBrush brush = CBrush();
        
        painter->SetFont(font);
        painter->SetPen(pen);

        Rect rect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());
        Vec4 padding = GetComputedLayoutPadding();

        Rect contentRect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());
        contentRect.min += padding.min;
        contentRect.max -= padding.max;

        Array<Rect> offsets{};
        Vec2 size = painter->CalculateTextOffsets(offsets, display);

        if (selectAll)
        {
            Rect lastCharacterRect = offsets.Top().Translate(padding.min);
            textScrollOffset = lastCharacterRect.min.x + lastCharacterRect.GetSize().width - contentRect.GetSize().width;
            selectionRange = RangeInt(0, offsets.GetSize() - 1);
            selectAll = false;
        }

        Vec2 textSize = painter->CalculateTextSize(display, isMultiline ? rect.GetSize().width : 0);
        Rect textRect = rect.Translate(Vec2(padding.left - textScrollOffset, rect.GetSize().height / 2 - textSize.height / 2));
        textRect.max -= Vec2(padding.left + padding.right, rect.GetSize().height / 2 - textSize.height / 2);

        cursorPos = Math::Clamp(cursorPos, 0, (int)text.GetLength() - 1);

        // Add text scroll offset to negate its effect from Clip Rect
        painter->PushClipRect(textRect.Translate(Vec2(textScrollOffset, 0)));
        painter->PushChildCoordinateSpace(rect.min);
        {
            if (IsFocussed() && IsInteractable()) // Edit mode
            {
                // Draw text selection background
                if (selectionRange.min >= 0 && selectionRange.max >= 0 && selectionRange.min <= selectionRange.max)
                {
                    Vec2 selectionStart = offsets[selectionRange.min].Translate(padding.min - Vec2(textScrollOffset, 0)).min;
                    Vec2 selectionEnd = offsets[selectionRange.max].Translate(padding.min - Vec2(textScrollOffset, 0)).min +
                        Vec2(offsets[selectionRange.max].GetSize().width, textSize.height);

                    brush.SetColor(textSelectionColor);
                    painter->SetBrush(brush);

                    painter->DrawRect(Rect(selectionStart, selectionEnd));
                }

                // Draw text

                painter->DrawText(display, textRect.min - rect.min);

                // Draw cursor

                constexpr f32 cursorWidth = 2.0f;
                Rect cursorRect = offsets[cursorPos].Translate(padding.min - Vec2(textScrollOffset, 0) - Vec2(cursorWidth / 2.0f, 0));
                cursorRect.max.x = cursorRect.min.x + cursorWidth;
                cursorRect.max.y = cursorRect.min.y + textSize.height;

                if (cursorState)
                {
                    brush.SetColor(Color::White());
                    painter->SetBrush(brush);

                    painter->DrawRect(cursorRect);
                }
            }
            else
            {
                painter->DrawText(display, textRect.min - rect.min);
            }
        }
        painter->PopChildCoordinateSpace();
        painter->PopClipRect();
    }

} // namespace CE::Widgets
