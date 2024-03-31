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
        String display = GetDisplayText();
        if (display.IsEmpty())
            display = " ";

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

    bool CTextInput::IsTextSelected()
    {
        return selectionRange.min >= 0 && selectionRange.max >= 0 && selectionRange.min <= selectionRange.max;
    }

    void CTextInput::OnTimerTick()
    {
        cursorState = !cursorState;
        if (isCursorMoving)
        {
            cursorState = true;
            isCursorMoving = false;
        }
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

        textScrollOffset = 0;
        timer->Stop();
        isEditing = false;
        cursorState = false;
        SetNeedsPaint();
    }

    void CTextInput::HandleEvent(CEvent* event)
    {
        Name fontName = computedStyle.properties[CStylePropertyType::FontName].string;

        f32 fontSize = 14;
        if (computedStyle.properties.KeyExists(CStylePropertyType::FontSize))
            fontSize = computedStyle.properties[CStylePropertyType::FontSize].single;

        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = (CMouseEvent*)event;
            Vec2 screenMousePos = mouseEvent->mousePos;
            Vec2 mouseDelta = mouseEvent->mousePos - mouseEvent->prevMousePos;
            Vec2 localMousePos = ScreenSpaceToLocalPoint(screenMousePos);
            Vec4 padding = GetComputedLayoutPadding();

            Renderer2D* renderer = GetRenderer();
            String display = text;

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

                if (selectedIdx == -1 && offsets.NonEmpty())
                {
                    Rect localPos = offsets.Top().Translate(padding.min - Vec2(textScrollOffset, 0));

                    if (localMousePos.x > localPos.max.x)
                    {
                        selectedIdx = offsets.GetSize();
                        cursorPos = selectedIdx;
                    }
                }

                if (selectedIdx != -1)
                {
                    timer->Reset();
                    timer->Start(cursorBlinkMillis);
                    cursorState = true;
                    isEditing = true;
                    originalText = text;
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

                if (selectionDistance > 0 && range.min == -1 && range.max >= 0 && range.max < offsets.GetSize())
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
                if (selectionDistance < 0 && range.max == -1 && range.min >= 0 && range.min < offsets.GetSize())
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
        else if (event->type == CEventType::KeyHeld || event->type == CEventType::KeyPress)
        {
            Renderer2D* renderer = GetRenderer();
            
            CKeyEvent* keyEvent = static_cast<CKeyEvent*>(event);

        	bool shiftPressed = EnumHasAnyFlags(keyEvent->modifier, KeyModifier::LShift | KeyModifier::RShift);
            bool capslock = EnumHasAnyFlags(keyEvent->modifier, KeyModifier::Caps);

            bool isUpperCase = shiftPressed != capslock;

            char c = 0;
            Vec4 padding = GetComputedLayoutPadding();

            Rect contentRect = Rect::FromSize(GetComputedLayoutTopLeft(), GetComputedLayoutSize());
            contentRect.min += padding.min;
            contentRect.max -= padding.max;

            // A valid text character
            if ((int)keyEvent->key >= (int)KeyCode::Space && (int)keyEvent->key <= (int)KeyCode::Z && IsEditing())
            {
                if (IsTextSelected())
                {
                    String newText = text;
                    newText.Remove(selectionRange.min, selectionRange.max - selectionRange.min + 1);

                    if (!inputValidator.IsValid() || inputValidator(newText))
                    {
                        this->text = newText;
                        cursorPos = selectionRange.min;

                        String displayText = "";
                        for (int i = 0; i < text.GetLength(); ++i)
                        {
                            if (isPassword)
                                displayText.Append('*');
                            else
                                displayText.Append(text[i]);
                        }

                        Array<Rect> offsets{};
                        Vec2 size = renderer->CalculateTextOffsets(offsets, displayText, fontSize, fontName);

                        selectionRange.min = selectionRange.max = -1;
                        textScrollOffset = 0;

                        SetNeedsLayout();
                        SetNeedsPaint();
                    }
                }

                c = (char)keyEvent->key;

                if (isUpperCase && c >= 'a' && c <= 'z')
                {
                    c += ('A' - 'a');
                }

                if (shiftPressed && (int)keyEvent->key >= (int)KeyCode::N0 && (int)keyEvent->key <= (int)KeyCode::N9)
                {
	                switch (keyEvent->key)
	                {
	                case KeyCode::N0:
                        c = ')';
                        break;
                    case KeyCode::N1:
                        c = '!';
                        break;
                    case KeyCode::N2:
                        c = '@';
                        break;
                    case KeyCode::N3:
                        c = '#';
                        break;
                    case KeyCode::N4:
                        c = '$';
                        break;
                    case KeyCode::N5:
                        c = '%';
                        break;
                    case KeyCode::N6:
                        c = '^';
                        break;
                    case KeyCode::N7:
                        c = '&';
                        break;
                    case KeyCode::N8:
                        c = '*';
                        break;
                    case KeyCode::N9:
                        c = '(';
                        break;
	                }
                }

                String newText = text;
                newText.InsertAt(c, cursorPos);

                String newDisplayText = "";
                for (int i = 0; i < newText.GetLength(); ++i)
                {
                    if (isPassword)
                        newDisplayText.Append('*');
                    else
                        newDisplayText.Append(newText[i]);
                }

                Array<Rect> offsets{};
                Vec2 size = renderer->CalculateTextOffsets(offsets, newDisplayText, fontSize, fontName);

                if (!inputValidator.IsValid() || inputValidator(newText))
                {
                    this->text = newText;

                    Rect cursorCharRect = offsets[cursorPos].Translate(padding.min);
                    if (cursorCharRect.min.x > contentRect.GetSize().width)
						textScrollOffset = cursorCharRect.min.x + cursorCharRect.GetSize().width - contentRect.GetSize().width;

                    cursorPos++;
                    SetNeedsLayout();
                    SetNeedsPaint();
                }
            }
            else if (keyEvent->key == KeyCode::Backspace && cursorPos > 0 && !IsTextSelected())
            {
                String newText = text;
                newText.Remove(cursorPos - 1, 1);

                if (!inputValidator.IsValid() || inputValidator(newText))
                {
                    this->text = newText;
                    cursorPos--;

                    String displayText = "";
                    for (int i = 0; i < text.GetLength(); ++i)
                    {
                        if (isPassword)
                            displayText.Append('*');
                        else
                            displayText.Append(text[i]);
                    }

                    Array<Rect> offsets{};
                    Vec2 size = renderer->CalculateTextOffsets(offsets, displayText, fontSize, fontName);

                    if (cursorPos > 0 && cursorPos < offsets.GetSize())
                    {
                        if (offsets[cursorPos - 1].min.x - textScrollOffset < 0)
                        {
                            textScrollOffset = offsets[cursorPos - 1].min.x;
                        }
                    }
                    else if (cursorPos <= 0)
                    {
                        textScrollOffset = 0;
                    }
                    
                    SetNeedsLayout();
                    SetNeedsPaint();
                }
            }
            else if ((keyEvent->key == KeyCode::Backspace || keyEvent->key == KeyCode::Delete) && IsTextSelected()) // A text selection got removed
            {
                String newText = text;
                newText.Remove(selectionRange.min, selectionRange.max - selectionRange.min + 1);

                if (!inputValidator.IsValid() || inputValidator(newText))
                {
                    this->text = newText;
                    cursorPos = selectionRange.min;

                    String displayText = "";
                    for (int i = 0; i < text.GetLength(); ++i)
                    {
                        if (isPassword)
                            displayText.Append('*');
                        else
                            displayText.Append(text[i]);
                    }

                    Array<Rect> offsets{};
                    Vec2 size = renderer->CalculateTextOffsets(offsets, displayText, fontSize, fontName);

                    selectionRange.min = selectionRange.max = -1;
                    textScrollOffset = 0;

                    SetNeedsLayout();
                    SetNeedsPaint();
                }
            }
            else if (keyEvent->key == KeyCode::Left)
            {
                isCursorMoving = true;
                String displayText = "";
                for (int i = 0; i < text.GetLength(); ++i)
                {
                    if (isPassword)
                        displayText.Append('*');
                    else
                        displayText.Append(text[i]);
                }

                int focusPos = 0;
	            if (EnumHasAnyFlags(keyEvent->modifier, KeyModifier::LShift | KeyModifier::RShift))
	            {
                    selectionRange.min = Math::Max(-1, selectionRange.min - 1);
                    cursorPos = Math::Max(0, cursorPos - 1);
                    focusPos = selectionRange.min;
	            }
                else
                {
                    selectionRange.min = selectionRange.max = -1; // Clear selection
                    cursorPos = Math::Max(0, cursorPos - 1);
                    focusPos = cursorPos;
                }

                Array<Rect> offsets{};
                Vec2 size = renderer->CalculateTextOffsets(offsets, displayText, fontSize, fontName);

                if (focusPos >= 0 && focusPos < offsets.GetSize())
                {
                    if (offsets[focusPos].min.x - textScrollOffset < 0)
                    {
                        textScrollOffset = offsets[focusPos].min.x;
                    }
                }

                SetNeedsLayout();
                SetNeedsPaint();
            }
            else if (keyEvent->key == KeyCode::Right)
            {
                isCursorMoving = true;

                String displayText = "";
                for (int i = 0; i < text.GetLength(); ++i)
                {
                    if (isPassword)
                        displayText.Append('*');
                    else
                        displayText.Append(text[i]);
                }

                int focusPos = 0;
                if (EnumHasAnyFlags(keyEvent->modifier, KeyModifier::LShift | KeyModifier::RShift))
                {
                    selectionRange.max = Math::Min((int)displayText.GetLength(), selectionRange.max + 1);
                    cursorPos = Math::Min((int)displayText.GetLength(), cursorPos + 1);
                    focusPos = cursorPos;
                }
                else
                {
                    selectionRange.min = selectionRange.max = -1; // Clear selection
                    cursorPos = Math::Min((int)displayText.GetLength(), cursorPos + 1);
                    focusPos = cursorPos;
                }

                Array<Rect> offsets{};
                Vec2 size = renderer->CalculateTextOffsets(offsets, displayText, fontSize, fontName);

                if (focusPos >= 0 && focusPos < offsets.GetSize())
                {
                    if (offsets[focusPos].min.x - textScrollOffset > contentRect.GetSize().width)
                    {
                        textScrollOffset = offsets[focusPos].min.x - contentRect.GetSize().width;
                    }
                }
                else if (focusPos >= offsets.GetSize())
                {
                    textScrollOffset = offsets.Top().max.x - contentRect.GetSize().width;
                }

                SetNeedsLayout();
                SetNeedsPaint();
            }
            else if (keyEvent->key == KeyCode::Return || keyEvent->key == KeyCode::KeypadEnter)
            {
                Unfocus();
            }
            else if (keyEvent->key == KeyCode::Escape)
            {
                text = originalText;
                Unfocus();
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

        if (selectAll && offsets.NonEmpty())
        {
            Rect lastCharacterRect = offsets.Top().Translate(padding.min);
            textScrollOffset = lastCharacterRect.min.x + lastCharacterRect.GetSize().width - contentRect.GetSize().width;
            selectionRange = RangeInt(0, offsets.GetSize() - 1);
        }

        selectAll = false;

        Vec2 textSize = painter->CalculateTextSize(display, isMultiline ? rect.GetSize().width : 0);
        Rect textRect = rect.Translate(Vec2(padding.left - textScrollOffset, rect.GetSize().height / 2 - textSize.height / 2));
        textRect.max -= Vec2(padding.left + padding.right, rect.GetSize().height / 2 - textSize.height / 2);

        cursorPos = Math::Clamp(cursorPos, 0, (int)text.GetLength());

        selectionRange.min = Math::Max(0, selectionRange.min);
        selectionRange.max = Math::Min(selectionRange.max, (int)offsets.GetSize() - 1);

        // Add text scroll offset to negate its effect from Clip Rect
        painter->PushClipRect(textRect.Translate(Vec2(textScrollOffset, 0)));
        painter->PushChildCoordinateSpace(rect.min);
        {
            if (isEditing) // Edit mode
            {
                // Draw text selection background
                if (IsTextSelected())
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
                Rect cursorRect = Rect();
                if (cursorPos >= 0 && cursorPos < offsets.GetSize())
                {
	                cursorRect = offsets[cursorPos].Translate(padding.min - Vec2(textScrollOffset, 0) - Vec2(cursorWidth / 2.0f, 0));
                }
                else if (cursorPos == offsets.GetSize() && offsets.NonEmpty())
                {
	                cursorRect = offsets.Top().Translate(padding.min - Vec2(textScrollOffset, 0) - Vec2(cursorWidth / 2.0f, 0) + 
					   Vec2(offsets.Top().GetSize().width, 0));
                }
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
