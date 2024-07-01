#include "FusionCore.h"

namespace CE
{
    FTextInputLabel::FTextInputLabel()
    {
        cursorTimer = CreateDefaultSubobject<FTimer>("Timer");
        cursorTimer->OnTimeOut(FUNCTION_BINDING(this, OnTimeOut));
    }

    void FTextInputLabel::CalculateIntrinsicSize()
    {
	    Super::CalculateIntrinsicSize();


    }

    void FTextInputLabel::OnFusionPropertyModified(const CE::Name& propertyName)
    {
	    Super::OnFusionPropertyModified(propertyName);

        static const CE::Name TextName = "Text";

        if (propertyName == TextName)
        {
            RecalculateCharacterOffsets();
        }
    }

    void FTextInputLabel::OnPaint(FPainter* painter)
    {
        if (characterOffsets.GetSize() != m_Text.GetLength())
        {
            RecalculateCharacterOffsets();
        }

        painter->PushChildCoordinateSpace(Matrix4x4::Translation(Vec3(-textScrollOffset, 0, 0)));

        if (IsEditing() && IsTextSelected() && selectionStart <= selectionEnd)
        {
            painter->SetPen(FPen());
            painter->SetBrush(m_SelectionColor);

            Range start = GetCharacterMinMax(selectionStart);
            Range end = GetCharacterMinMax(selectionEnd);

            Rect rect = Rect(start.min, 0, end.max, GetComputedSize().height);

            painter->DrawRect(rect);
        }

	    Super::OnPaint(painter);

        if (IsEditing() && cursorState)
        {
            painter->SetPen(FPen(Color::White(), 1.2f));
            painter->SetBrush(FBrush());

            f32 posX = GetCharacterMinMax(cursorPos).min;
            Vec2 lineStart = Vec2(posX, 0);
            Vec2 lineEnd = lineStart + Vec2(0, GetComputedSize().height - m_Padding.bottom - m_Padding.top);

            painter->DrawLine(lineStart, lineEnd);
        }

        painter->PopChildCoordinateSpace();
    }

    void FTextInputLabel::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent())
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);
            auto app = FusionApplication::Get();

            Vec2 mousePos = mouseEvent->mousePosition;
            Vec2 localMousePos = mousePos - globalPosition;

            if (event->type == FEventType::MouseEnter)
            {
                app->PushCursor(SystemCursor::IBeam);
            }
            else if (event->type == FEventType::MouseLeave)
            {
                app->PopCursor();
            }
            else if (event->type == FEventType::MousePress && mouseEvent->buttons == MouseButtonMask::Left)
            {
                OnMouseClick(localMousePos, mouseEvent->isDoubleClick);
            }
        }
        else if (!event->isConsumed && (event->type == FEventType::KeyHeld || event->type == FEventType::KeyPress))
        {
            FKeyEvent* keyEvent = static_cast<FKeyEvent*>(event);

            event->Consume(this);

            bool shiftPressed = EnumHasAnyFlags(keyEvent->modifiers, KeyModifier::LShift | KeyModifier::RShift);
            bool capslock = EnumHasAnyFlags(keyEvent->modifiers, KeyModifier::Caps);
            bool ctrl = EnumHasFlag(keyEvent->modifiers, KeyModifier::Ctrl);

            bool isUpperCase = shiftPressed != capslock;
            char c = 0;

            if ((int)keyEvent->key >= (int)KeyCode::Space && (int)keyEvent->key <= (int)KeyCode::Z && IsEditing())
            {
                c = (char)keyEvent->key;

                if (isUpperCase && c >= 'a' && c <= 'z')
                {
                    c += ('A' - 'a');
                }

                if (shiftPressed)
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
                    case KeyCode::Semicolon:
                        c = ':';
                        break;
                    case KeyCode::LeftBracket:
                        c = '{';
                        break;
                    case KeyCode::RightBracket:
                        c = '}';
                        break;
                    case KeyCode::Equals:
                        c = '+';
                        break;
                    case KeyCode::Minus:
                        c = '_';
                        break;
                    case KeyCode::Backquote:
                        c = '~';
                        break;
                    case KeyCode::Comma:
                        c = '<';
                        break;
                    case KeyCode::Period:
                        c = '>';
                        break;
                    case KeyCode::Slash:
                        c = '?';
                        break;
                    case KeyCode::Quote:
                        c = '\"';
                        break;
                    }
                }

                bool isCommand = false;

                if (ctrl) // Check for commands
                {
                    if (keyEvent->key == KeyCode::X && IsTextSelected()) // Cut
                    {
                        String selected = GetSelectedText();
                        PlatformApplication::Get()->SetClipboardText(selected);
                        RemoveSelectedRange();

                        isCommand = true;
                    }
                    else if (keyEvent->key == KeyCode::C && IsTextSelected()) // Copy
                    {
                        String selected = GetSelectedText();
                        PlatformApplication::Get()->SetClipboardText(selected);

                        isCommand = true;
                    }
                    else if (keyEvent->key == KeyCode::V) // Paste
                    {
                        String textToPaste = "";
                        if (PlatformApplication::Get()->HasClipboardText())
                        {
                            textToPaste = PlatformApplication::Get()->GetClipboardText();
                            if (IsTextSelected())
                                RemoveSelectedRange();
                            InsertAt(textToPaste, cursorPos);
                        }

                        isCommand = true;
                    }
                    else if (keyEvent->key == KeyCode::A)
                    {
                        SelectAll();

                        isCommand = true;
                    }
                    else if (IsTextSelected())
                    {
                        RemoveSelectedRange();
                    }
                }
                else if (IsTextSelected())
                {
                    RemoveSelectedRange();
                }

                if (!isCommand)
                {
                    String str = "";
                    str.Append(c);

                    InsertAt(str, cursorPos);
                    ScrollTo(cursorPos);
                }
            }
            else if ((int)keyEvent->key >= (int)KeyCode::KeypadDivide && (int)keyEvent->key <= (int)KeyCode::KeypadPeriod &&
                keyEvent->key != KeyCode::KeypadEnter &&
                EnumHasFlag(keyEvent->modifiers, KeyModifier::Num))
            {
                switch (keyEvent->key)
                {
                case KeyCode::KeypadDivide:
                    c = '/';
                    break;
                case KeyCode::KeypadPlus:
                    c = '+';
                    break;
                case KeyCode::KeypadMinus:
                    c = '-';
                    break;
                case KeyCode::KeypadMultiply:
                    c = '*';
                    break;
                case KeyCode::Keypad0:
                    c = '0';
                    break;
                case KeyCode::Keypad1:
                    c = '1';
                    break;
                case KeyCode::Keypad2:
                    c = '2';
                    break;
                case KeyCode::Keypad3:
                    c = '3';
                    break;
                case KeyCode::Keypad4:
                    c = '4';
                    break;
                case KeyCode::Keypad5:
                    c = '5';
                    break;
                case KeyCode::Keypad6:
                    c = '6';
                    break;
                case KeyCode::Keypad7:
                    c = '7';
                    break;
                case KeyCode::Keypad8:
                    c = '8';
                    break;
                case KeyCode::Keypad9:
                    c = '9';
                    break;
                case KeyCode::KeypadPeriod:
                    c = '.';
                    break;
                }

                if (c != 0)
                {
                    String str = "";
                    str.Append(c);

                    InsertAt(str, cursorPos);
                }
            }
            else if (IsTextSelected() && (keyEvent->key == KeyCode::Backspace || keyEvent->key == KeyCode::Delete))
            {
                RemoveSelectedRange();
            }
            else if (keyEvent->key == KeyCode::Backspace)
            {
                RemoveRange(cursorPos - 1, 1);
            }
            else if (keyEvent->key == KeyCode::Delete)
            {
                RemoveRange(cursorPos, 1);
            }
            else if (keyEvent->key == KeyCode::Left)
            {
                int newCursorPos = cursorPos - 1;

                if (shiftPressed)
                {
                    int startIdx = selectionStart;
                    int endIdx = selectionEnd;
                    if (!IsTextSelected())
                    {
                        startIdx = cursorPos;
                        endIdx = cursorPos - 1;
                    }

                    if (IsTextSelected() && cursorPos > startIdx)
                    {
                        SelectRange(startIdx, endIdx - 1);
                        ScrollTo(endIdx - 1);
                    }
                    else
                    {
                        SelectRange(startIdx - 1, endIdx);
                        ScrollTo(startIdx - 1);

                        cursorPos = endIdx;
                    }
                }
                else
                {
	                if (IsTextSelected())
	                {
                        newCursorPos = selectionStart - 1;
                        DeselectAll();
	                }

                    ScrollTo(newCursorPos);
                }

                cursorState = true;
                cursorTimer->Reset();

                SetCursorPos(newCursorPos);
            }
            else if (keyEvent->key == KeyCode::Right)
            {
                int newCursorPos = cursorPos + 1;

                if (shiftPressed)
                {
                    int startIdx = selectionStart;
                    int endIdx = selectionEnd;
                    if (!IsTextSelected())
                    {
                        startIdx = cursorPos;
                        endIdx = cursorPos - 1;
                    }

                    if (IsTextSelected() && cursorPos <= endIdx && endIdx < characterOffsets.GetSize())
                    {
                        SelectRange(startIdx + 1, endIdx);
                        ScrollTo(startIdx + 1);
                    }
                    else
                    {
                        SelectRange(startIdx, endIdx + 1);
                        ScrollTo(endIdx + 1);
                    }

                    SetCursorPos(selectionEnd);
                }
                else
                {
                    if (IsTextSelected())
                    {
                        DeselectAll();
                    }

                    ScrollTo(newCursorPos);
                }

                cursorState = true;
                cursorTimer->Reset();

                SetCursorPos(newCursorPos);
            }
            else if (keyEvent->key == KeyCode::Return || keyEvent->key == KeyCode::KeypadEnter)
            {
                StopEditing(false);
                Unfocus();
            }
            else if (keyEvent->key == KeyCode::Escape)
            {
                StopEditing(true);
                Unfocus();
            }
        }
        else if (event->type == FEventType::FocusChanged)
        {
            FFocusEvent* focusEvent = static_cast<FFocusEvent*>(event);

            if (focusEvent->LostFocus())
            {
                StopEditing();
            }
        }

	    Super::HandleEvent(event);
    }

    void FTextInputLabel::RecalculateCharacterOffsets()
    {
        FFusionContext* context = GetContext();
        if (!context)
            return;

        FPainter* painter = context->GetPainter();
        if (!painter)
            return;

        textSize = painter->CalculateCharacterOffsets(characterOffsets, Text(), Font());
    }

    void FTextInputLabel::OnTimeOut()
    {
        cursorState = !cursorState;
        MarkDirty();
    }

    void FTextInputLabel::OnMouseClick(Vec2 localPos, bool isDoubleClick)
    {
	    for (int i = 0; i < characterOffsets.GetSize(); ++i)
	    {
            if ((i > 0 && localPos.x < characterOffsets[i].min.x) || (i < characterOffsets.GetSize() - 1 && localPos.x > characterOffsets[i].max.x))
                continue;

            f32 halfWayPos = (characterOffsets[i].min.x + characterOffsets[i].max.x) / 2.0f;
            if (localPos.x <= halfWayPos)
                cursorPos = i;
            else
                cursorPos = i + 1;

            cursorState = true;

            if (!IsEditing())
            {
                StartEditing();
            }
            else
            {
                MarkDirty();
            }

            if (isDoubleClick)
            {
                SelectAll();
                ScrollTo(characterOffsets.GetSize());
            }
            else if (IsTextSelected())
            {
                DeselectAll();
            }
            
            break;
	    }
    }

    Range FTextInputLabel::GetCharacterMinMax(int characterIndex)
    {
        if (characterOffsets.IsEmpty())
        {
            return Range(0, 0);
        }

        if (characterIndex < characterOffsets.GetSize())
        {
            return Range(characterOffsets[characterIndex].min.x, characterOffsets[characterIndex].max.x);
        }

        return Range(characterOffsets.Top().max.x, characterOffsets.Top().max.x + characterOffsets.Top().GetSize().width);
    }

    bool FTextInputLabel::IsEditing()
    {
        return textInput->IsEditing();
    }

    void FTextInputLabel::StartEditing()
    {
        originalText = m_Text;

        cursorTimer->Reset();
        cursorState = true;
        cursorTimer->Start(500);

        textInput->SetEditingInternal(true);
        MarkDirty();
    }

    void FTextInputLabel::StopEditing(bool restoreOriginal)
    {
        if (!textInput->IsEditing())
            return;

        if (restoreOriginal)
        {
            m_Text = originalText;
        }

        cursorTimer->Stop();
        cursorState = false;
        
        textInput->SetEditingInternal(false);
        ScrollTo(0);
        DeselectAll();

        MarkLayoutDirty();

        if (!restoreOriginal && m_TextBinding.write.IsBound())
        {
            m_TextBinding.write.Invoke(m_Text);
        }
    }

    void FTextInputLabel::SetCursorPos(int newCursorPos)
    {
        cursorPos = newCursorPos;
        cursorPos = Math::Clamp<int>(cursorPos, 0, characterOffsets.GetSize());

        MarkDirty();
    }

    void FTextInputLabel::ScrollTo(int charIndex)
    {
        charIndex = Math::Clamp<int>(charIndex, 0, characterOffsets.GetSize());

        Range charRange = GetCharacterMinMax(charIndex);

        if (charRange.min - textScrollOffset < 0)
        {
            textScrollOffset = charRange.min;

            MarkDirty();
        }
        else if (charRange.max - textScrollOffset > computedSize.width)
        {
            textScrollOffset = charRange.max - computedSize.width;

            MarkDirty();
        }
    }

    void FTextInputLabel::InsertAt(const String& string, int insertPos)
    {
        if (insertPos < 0 || insertPos > m_Text.GetLength())
            return;

        String newText = m_Text;
        newText.Reserve(newText.GetLength() + string.GetLength());
        for (int i = 0; i < string.GetLength(); ++i)
        {
            newText.InsertAt(string[i], insertPos + i);
        }

        m_Text = newText;
        cursorPos = insertPos + string.GetLength();

        MarkLayoutDirty();
    }

    void FTextInputLabel::InsertAt(char character, int insertPos)
    {
        if (insertPos < 0 || insertPos > m_Text.GetLength())
            return;

        String newText = m_Text;
        newText.Reserve(newText.GetLength() + 1);
        newText.InsertAt(character, insertPos);

        m_Text = newText;
        cursorPos = insertPos + 1;

        MarkLayoutDirty();
    }

    void FTextInputLabel::RemoveRange(int startIndex, int count)
    {
        if (startIndex < 0 || startIndex > m_Text.GetLength())
            return;

        count = Math::Min<int>(count, m_Text.GetLength() - startIndex);

        String newText = m_Text;
        newText.Remove(startIndex, count);

        m_Text = newText;

        SetCursorPos(startIndex);

        MarkLayoutDirty();
    }

    void FTextInputLabel::RemoveSelectedRange()
    {
        if (IsTextSelected() && selectionStart <= selectionEnd)
        {
            RemoveRange(selectionStart, selectionEnd - selectionStart + 1);
            DeselectAll();

            ScrollTo(selectionStart);
        }
    }

    String FTextInputLabel::GetSelectedText()
    {
        if (!IsTextSelected())
        {
            return "";
        }

        return m_Text.GetSubstring(selectionStart, selectionEnd - selectionStart + 1);
    }

    void FTextInputLabel::SelectAll()
    {
        SelectRange(0, characterOffsets.GetSize());
    }

    void FTextInputLabel::SelectRange(int startIndex, int endIndex)
    {
        isSelectionActive = true;

        selectionStart = Math::Max(0, startIndex);
        selectionEnd = Math::Min<int>(endIndex, characterOffsets.GetSize());
        MarkDirty();
    }

    void FTextInputLabel::DeselectAll()
    {
        isSelectionActive = false;
        MarkDirty();
    }

    FTextInput::FTextInput()
    {
        m_Padding = Vec4(7.5f, 5, 7.5f, 5);
    }

    void FTextInput::OnLostFocus()
    {
	    Super::OnLostFocus();

        inputLabel->StopEditing();
    }

    void FTextInput::Construct()
    {
	    Super::Construct();
        
        Child(
            FAssignNew(FHorizontalStack, stack)
            .ContentVAlign(VAlign::Center)
            .Name("TextInputHStack")
            (
                FNew(FStyledWidget)
                .ClipShape(FRectangle())
                .FillRatio(1.0f)
                .Name("InputHolder")
                (
                    FAssignNew(FTextInputLabel, inputLabel)
                    .Text("")
                    .WordWrap(FWordWrap::NoWrap)
                    .FontSize(13)
                    .Foreground(Color::White())
                    .HAlign(HAlign::Fill)
                    .VAlign(VAlign::Fill)
                    .Name("TextInputLabel")
                )
            )
        );

        inputLabel->textInput = this;
    }

    void FTextInput::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent() && event->sender == this)
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);
            auto app = FusionApplication::Get();

            if (mouseEvent->type == FEventType::MouseEnter)
            {
                app->PushCursor(SystemCursor::IBeam);
            }
            else if (mouseEvent->type == FEventType::MouseLeave)
            {
                app->PopCursor();
            }
            else if (mouseEvent->type == FEventType::MousePress && mouseEvent->buttons == MouseButtonMask::Left)
            {
                Vec2 localPos = mouseEvent->mousePosition - inputLabel->globalPosition;
                inputLabel->OnMouseClick(localPos, mouseEvent->isDoubleClick);
            }
        }

	    Super::HandleEvent(event);
    }

    void FTextInput::SetEditingInternal(bool editing)
    {
        if (IsEditing() == editing)
            return;

        if (editing)
        {
            state |= FTextInputState::Editing;
        }
        else
        {
            state &= ~FTextInputState::Editing;
        }

        ApplyStyle();
    }

    FTextInput& FTextInput::LeftSlot(FWidget& content)
    {
        stack->InsertChild(0, &content);
        return *this;
    }

} // namespace CE
