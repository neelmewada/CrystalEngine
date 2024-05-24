#include "EditorSystem.h"

namespace CE::Editor
{

	void ColorFieldEditor::BindField(FieldType* field, void* instance)
	{

	}

	void ColorFieldEditor::Construct()
    {
        Super::Construct();

        colorInput = CreateObject<ColorInput>(this, "ColorInput");
        colorInput->SetValue(Color(1.0f, 0, 0, 0));
    }

} // namespace CE::Editor
