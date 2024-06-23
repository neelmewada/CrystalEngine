#include "FusionCore.h"

namespace CE
{
    FTextInputLabel::FTextInputLabel()
    {
	    
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
            m_OnTextPropertyUpdated();
        }
    }

    FTextInput::FTextInput()
    {
        m_Padding = Vec4(7.5f, 5, 7.5f, 5);

        cursorTimer = CreateDefaultSubobject<FTimer>("CursorTimer");
    }

    const String& FTextInput::GetText() const
    {
        return inputLabel->GetText();
    }

    void FTextInput::OnLostFocus()
    {
	    Super::OnLostFocus();

        SetEditing(false);
    }

    void FTextInput::Construct()
    {
	    Super::Construct();

        cursorTimer->OnTimeOut(FUNCTION_BINDING(this, OnTimeOut));
        
        Child(
            FNew(FHorizontalStack)
            .ContentVAlign(VAlign::Center)
            (
                FAssignNew(FCompoundWidget, leftSlot),

                FNew(FStyledWidget)
                .ClipShape(FRectangle())
                .Name("InputHolder")
                (
                    FAssignNew(FTextInputLabel, inputLabel)
                    .OnTextPropertyUpdated(FUNCTION_BINDING(this, RecalculateCharacterOffsets))
                    .Text("")
                    .WordWrap(FWordWrap::NoWrap)
                    .FontSize(13)
                    .Foreground(Color::White())
                    .Name("TextInputLabel")
                )
            )
        );
    }

    void FTextInput::OnFusionPropertyModified(const CE::Name& propertyName)
    {
        Super::OnFusionPropertyModified(propertyName);

        
    }

    void FTextInput::OnPaintContent(FPainter* painter)
    {
        Super::OnPaintContent(painter);

        if (characterOffsets.GetSize() != GetText().GetLength())
        {
            RecalculateCharacterOffsets();
        }

        if (IsEditing())
        {
            cursorPos = Math::Clamp<int>(cursorPos, 0, GetText().GetLength());

            if (IsSelectionActive() && selectionStart <= selectionEnd && selectionStart < characterOffsets.GetSize())
            {
                painter->SetBrush(m_SelectionColor);
                painter->SetPen(FPen());

                f32 startX = 0, endX = 0;
                
                startX = characterOffsets[selectionStart].min.x;
                if (selectionEnd < characterOffsets.GetSize())
                    endX = characterOffsets[selectionEnd].min.x;
                else
                    endX = characterOffsets[selectionEnd - 1].max.x;

                painter->DrawRect(Rect::FromSize(computedPosition + m_Padding.min + Vec2(startX, 0),
                    Vec2(endX - startX, computedSize.height - m_Padding.bottom - m_Padding.top)));
            }

            if (cursorState)
            {
                painter->SetPen(FPen(Color::White(), 1.5f));

                FFontMetrics metrics = painter->GetFontMetrics(inputLabel->GetFont());

                f32 posX = 0;
                if (cursorPos < characterOffsets.GetSize())
                    posX = characterOffsets[cursorPos].min.x;
                else
                    posX = characterOffsets[cursorPos - 1].max.x;

                Vec2 lineStart = computedPosition + m_Padding.min + Vec2(posX, 0);
                Vec2 lineEnd = lineStart;
                lineEnd.y += computedSize.height - m_Padding.bottom - m_Padding.top;

                painter->DrawLine(lineStart, lineEnd);
            }
        }
    }

    void FTextInput::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent() && event->sender == this)
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);
            Vec2 labelSpacePos = mouseEvent->mousePosition - inputLabel->globalPosition - 
                Vec2(inputLabel->m_Padding.left, inputLabel->m_Padding.top);

            auto app = FusionApplication::Get();

            if (characterOffsets.GetSize() != inputLabel->GetText().GetLength())
            {
                RecalculateCharacterOffsets();
            }

            if (mouseEvent->type == FEventType::MousePress && mouseEvent->buttons == MouseButtonMask::Left)
            {
	            for (int i = 0; i < characterOffsets.GetSize(); ++i)
	            {
		            if ((i > 0 && labelSpacePos.x < characterOffsets[i].min.x) || (i < characterOffsets.GetSize() - 1 && labelSpacePos.x > characterOffsets[i].max.x))
						continue;

                    f32 halfWayPos = (characterOffsets[i].min.x + characterOffsets[i].max.x) / 2.0f;
                    if (labelSpacePos.x <= halfWayPos)
                        cursorPos = i;
                    else
                        cursorPos = i + 1;

                    if (mouseEvent->isDoubleClick && characterOffsets.NonEmpty())
                    {
                        SelectRange(0, characterOffsets.GetSize());
                    }
                    else
                    {
                        DeselectAll();

                        SetEditing(true);
                        SetCursorPos(cursorPos);
                    }

                    cursorState = true;

                    break;
	            }
            }
            else if (mouseEvent->type == FEventType::MouseEnter && mouseEvent->sender == this)
            {
                app->PushCursor(SystemCursor::IBeam);
            }
            else if (mouseEvent->type == FEventType::MouseLeave && mouseEvent->sender == this)
            {
                app->PopCursor();
            }
            else if (mouseEvent->type == FEventType::DragBegin)
            {
                FDragEvent* dragEvent = static_cast<FDragEvent*>(event);

                dragEvent->draggedWidget = this;
                dragEvent->Consume(this);
                
            }
            else if (mouseEvent->type == FEventType::DragMove)
            {
                FDragEvent* dragEvent = static_cast<FDragEvent*>(event);


            }
            else if (mouseEvent->type == FEventType::DragEnd)
            {
                FDragEvent* dragEvent = static_cast<FDragEvent*>(event);

            }
        }
        else if (event->IsKeyEvent() && event->sender == this && IsEditing())
        {
            FKeyEvent* keyEvent = static_cast<FKeyEvent*>(event);

            if (keyEvent->type == FEventType::KeyPress)
            {
                
            }

            keyEvent->Consume(this);
        }

        Super::HandleEvent(event);
    }

    void FTextInput::RecalculateCharacterOffsets()
    {
        FFusionContext* context = GetContext();
        if (!context)
            return;

        FPainter* painter = context->GetPainter();
        if (!painter)
            return;

        textSize = painter->CalculateCharacterOffsets(characterOffsets, inputLabel->GetText(), inputLabel->GetFont());
    }

    void FTextInput::TryRecalculateCharacterOffsets()
    {
        if (characterOffsets.GetSize() != GetText().GetLength())
        {
            RecalculateCharacterOffsets();
        }
    }

    void FTextInput::OnTimeOut()
    {
        cursorState = !cursorState;
        MarkDirty();
    }

    void FTextInput::SetEditing(bool edit)
    {
        if (IsEditing() == edit)
            return;

        if (edit)
        {
            cursorTimer->Reset();
            cursorTimer->Start(500);
            cursorState = true;
            state |= FTextInputState::Editing;
        }
        else
        {
            cursorTimer->Reset();
            cursorTimer->Stop();
            state &= ~FTextInputState::Editing;
        }

        MarkDirty();
        ApplyStyle();
    }

    void FTextInput::SetCursorPos(int cursorPos)
    {
        cursorPos = Math::Clamp<int>(cursorPos, 0, characterOffsets.GetSize());

        this->cursorPos = cursorPos;
        ScrollTo(cursorPos);
    }

    void FTextInput::ScrollTo(int cursorPos)
    {
        cursorPos = Math::Clamp<int>(cursorPos, 0, characterOffsets.GetSize());
        f32 characterStartX = cursorPos < characterOffsets.GetSize() ? characterOffsets[cursorPos].min.x : characterOffsets[cursorPos - 1].max.x;
        f32 characterEndX = cursorPos < characterOffsets.GetSize() ? characterOffsets[cursorPos].max.x : characterOffsets[cursorPos - 1].max.x;

        f32 contentWidth = GetComputedSize().width - m_Padding.left - m_Padding.right;
        if (characterStartX - scrollOffset < 0)
        {
            scrollOffset = characterStartX;
        }
        else if (characterEndX - scrollOffset > contentWidth)
        {
            scrollOffset = characterEndX - contentWidth;
        }

        inputLabel->Translation(Vec2(-scrollOffset, 0));

        MarkDirty();
    }

    void FTextInput::InsertAt(const String& string, int insertPos)
    {

    }

    void FTextInput::RemoveRange(int startIndex, int count)
    {

    }

    void FTextInput::SelectRange(int startIndex, int endIndex)
    {
        isSelectionActive = true;

        selectionStart = Math::Min(startIndex, endIndex);
        selectionEnd = Math::Max(startIndex, endIndex);

        MarkDirty();
    }

    void FTextInput::DeselectAll()
    {
        if (!isSelectionActive)
            return;

        isSelectionActive = false;

        MarkDirty();
    }

    FTextInput& FTextInput::Text(const String& value)
    {
        if (inputLabel->GetText() == value)
            return *this;

        RecalculateCharacterOffsets();
        inputLabel->Text(value);
        return *this;
    }

    FTextInput& FTextInput::Foreground(const Color& value)
    {
        inputLabel->Foreground(value);
        return *this;
    }

    FTextInput& FTextInput::LeftSlot(FWidget& content)
    {
        leftSlot->Child(content);
        return *this;
    }

} // namespace CE
