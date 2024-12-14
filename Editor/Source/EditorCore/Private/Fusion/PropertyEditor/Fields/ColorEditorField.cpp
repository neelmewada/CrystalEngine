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
            .Name("Border")
            (
                FNew(FHorizontalStack)
                (
                    FNew(FStyledWidget)
                    .Background(value.WithAlpha(1.0f))
                    .BackgroundShape(FRoundedRectangle(Rounding, 0, 0, Rounding))
                    .Width(25)
                    .Height(10),

                    FNew(FOverlayStack)
                    .Width(25)
                    .Height(20)
                    (
                        FNew(FStyledWidget)
                        .Background(gridBrush)
                        .BackgroundShape(FRoundedRectangle(0, Rounding, Rounding, 0))
                        .Width(25)
                        .Height(10),

                        FNew(FStyledWidget)
                        .Background(value)
                        .BackgroundShape(FRoundedRectangle(0, Rounding, Rounding, 0))
                        .Width(25)
                        .Height(10)
                    )
                )
            )
        );
    }

    void ColorEditorField::OnPaint(FPainter* painter)
    {
        Super::OnPaint(painter);

        painter->SetPen(FPen(Color::RGBA(56, 56, 56), 1));
        painter->SetBrush(FBrush());

        painter->DrawRect(Rect::FromSize(Vec2(), GetComputedSize()));
    }


    bool ColorEditorField::CanBind(FieldType* field)
    {
        return field->GetDeclarationTypeId() == TYPEID(Color);
    }

    void ColorEditorField::UpdateValue()
    {
        if (field == nullptr || instances.IsEmpty() || instances[0] == nullptr)
            return;

        value = field->GetFieldValue<Color>(instances[0]);
    }

}

