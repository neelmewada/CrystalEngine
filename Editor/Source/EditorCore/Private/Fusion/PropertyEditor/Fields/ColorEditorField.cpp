#include "EditorCore.h"

namespace CE::Editor
{
    static constexpr f32 Rounding = 2.5f;

    ColorEditorField::ColorEditorField()
    {

    }

    void ColorEditorField::Construct()
    {
        Super::Construct();

        FBrush gridBrush = FBrush("/Engine/Resources/Icons/TransparentPattern");
        gridBrush.SetBrushTiling(FBrushTiling::TileXY);
        gridBrush.SetBrushSize(Vec2(16, 16));

        Child(
            FNew(FStyledWidget)
            .VAlign(VAlign::Fill)
            .HAlign(HAlign::Fill)
            .Name("Border")
            (
                FNew(FHorizontalStack)
                .ContentVAlign(VAlign::Fill)
                .VAlign(VAlign::Fill)
                .Height(20)
                (
                    FAssignNew(FStyledWidget, left)
                    .Background(value.WithAlpha(1.0f))
                    .BackgroundShape(FRoundedRectangle(Rounding, 0, 0, Rounding))
                    .VAlign(VAlign::Fill)
                    .Width(40),

                    FNew(FOverlayStack)
                    .ContentVAlign(VAlign::Fill)
                    .VAlign(VAlign::Fill)
                    .Width(40)
                    (
                        FNew(FStyledWidget)
                        .Background(gridBrush)
                        .BackgroundShape(FRoundedRectangle(0, Rounding, Rounding, 0))
                        .VAlign(VAlign::Fill)
                        .Width(40),

                        FAssignNew(FStyledWidget, right)
                        .Background(value)
                        .BackgroundShape(FRoundedRectangle(0, Rounding, Rounding, 0))
                        .VAlign(VAlign::Fill)
                        .Width(40)
                    )
                )
            )
        );
    }

    void ColorEditorField::OnPaint(FPainter* painter)
    {
        Super::OnPaint(painter);

        auto style = EditorStyle::Get()->textInput;

        Color penColor = style->borderColor;
        if (isMouseInside && isPressed)
            penColor = style->editingBorderColor;
        else if (isMouseInside)
            penColor = style->hoverBorderColor;

        painter->SetPen(FPen(penColor, 1));
        painter->SetBrush(FBrush());

        f32 offset = 1;

        painter->DrawRoundedRect(Rect::FromSize(Vec2(-offset, -offset), GetComputedSize() + Vec2(offset, offset) * 2), 
            Vec4(1, 1, 1, 1) * Rounding);
    }

    void ColorEditorField::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent())
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MouseEnter)
            {
                isMouseInside = true;
                MarkDirty();
            }
            else if (mouseEvent->type == FEventType::MouseLeave)
            {
                isMouseInside = false;
                MarkDirty();
            }
            else if (mouseEvent->type == FEventType::MousePress)
            {
                isPressed = true;
                MarkDirty();
            }
            else if (mouseEvent->type == FEventType::MouseRelease && mouseEvent->isInside)
            {
                isPressed = false;
                MarkDirty();

                WeakRef<ColorEditorField> self = this;

                Ref<ColorPickerTool> colorPicker = ColorPickerTool::Open(m_History.Lock());

                if (IsBound())
                {
                    colorPicker->SetOriginalColor(GetValueDirect<Color>());
                    colorPicker->SetColor(GetValueDirect<Color>());
                }

                colorPicker->OnColorChanged([self](ColorPickerTool* tool)
                    {
                        if (Ref<ColorEditorField> lock = self.Lock())
                        {
	                        if (self->IsBound())
	                        {
                                self->SetValueDirect(tool->GetColor());

                                self->UpdateValue();
	                        }
                            else
                            {
                                self->SetColorValue(tool->GetColor());
                            }
                        }
                    });

                CE::Name fieldPath = this->relativeFieldPath;

                WeakRef<Object> target = self->targets[0];

                colorPicker->OnColorAccepted([self, fieldPath, target](ColorPickerTool* tool)
                    {
                        if (auto lock = self.Lock(); auto targetObj = target.Lock())
                        {
                            if (self->IsBound())
                            {
                                Color initialColor = tool->GetOriginalColor();
                                Color newColor = tool->GetColor();

                                if (auto history = self->m_History.Lock())
                                {
                                    history->PerformOperation("Edit Color Field", targetObj, fieldPath, initialColor, newColor);
                                }

                                self->UpdateValue();
                            }
                            else
                            {
                                self->SetColorValue(tool->GetColor());
                            }
                        }
                    });
            }
            else if (mouseEvent->type == FEventType::MouseRelease)
            {
                isPressed = false;
                MarkDirty();
            }
        }

	    Super::HandleEvent(event);
    }


    bool ColorEditorField::CanBind(FieldType* field)
    {
        return field->GetDeclarationTypeId() == TYPEID(Color);
    }

    void ColorEditorField::SetColorValue(const Color& color)
    {
        this->value = color;

        left->Background(value.WithAlpha(1.0f));
        right->Background(value);
    }

    void ColorEditorField::UpdateValue()
    {
        if (!IsBound())
        {
	        return;
        }

        value = GetValueDirect<Color>();

        left->Background(value.WithAlpha(1.0f));
        right->Background(value);
    }

}

