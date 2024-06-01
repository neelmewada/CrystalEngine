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
        
        colorInput->onColorSelected += [this, field, instance](const Color& newColor)
            {
                field->SetFieldValue<Color>(instance, newColor);
                colorInput->SetValue(newColor);

                onValueUpdated();
            };
	}

	void ColorFieldEditor::Construct()
    {
        Super::Construct();

        colorInput = CreateObject<ColorInput>(this, "ColorInput");
        colorInput->SetValue(Color(1.0f, 0, 0, 0.5f));
    }

} // namespace CE::Editor
