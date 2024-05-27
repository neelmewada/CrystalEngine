#include "EditorSystem.h"

namespace CE::Editor
{

	void ColorFieldEditor::BindField(FieldType* field, void* instance)
	{
        if (!field || !instance)
            return;

        if (field->GetDeclarationTypeId() != TYPEID(Color))
            return;

        colorInput->SetValue(field->GetFieldValue<Color>(instance));

        Bind(colorInput, MEMBER_FUNCTION(ColorInput, OnColorSelectSignal), [this, field, instance](Color newColor)
            {
                field->SetFieldValue<Color>(instance, newColor);
                colorInput->SetValue(newColor);
            });
	}

	void ColorFieldEditor::Construct()
    {
        Super::Construct();

        colorInput = CreateObject<ColorInput>(this, "ColorInput");
        colorInput->SetValue(Color(1.0f, 0, 0, 0.5f));
    }

} // namespace CE::Editor
