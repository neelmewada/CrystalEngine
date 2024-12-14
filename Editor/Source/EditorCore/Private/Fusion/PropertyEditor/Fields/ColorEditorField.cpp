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

                    FNew(FStyledWidget)
                    .Background(value)
                    .BackgroundShape(FRoundedRectangle(0, Rounding, Rounding, 0))
                    .Width(25)
                    .Height(10)
                )
            )
        );
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

