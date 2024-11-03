#include "EditorCore.h"

namespace CE::Editor
{

    PropertyEditorRegistry::PropertyEditorRegistry()
    {

    }

    PropertyEditorRegistry::~PropertyEditorRegistry()
    {
        
    }

    PropertyEditorRegistry& PropertyEditorRegistry::Get()
    {
        static PropertyEditorRegistry instance{};
        return instance;
    }

    PropertyEditor* PropertyEditorRegistry::Create(FieldType* field, Object* target)
    {
        thread_local Array targets = { target };
        targets[0] = target;

        return Create(field, targets);
    }

    PropertyEditor* PropertyEditorRegistry::Create(FieldType* field, const Array<Object*>& targets)
    {
        PropertyEditor* editor = nullptr;
        Object* transient = GetTransient(MODULE_NAME);

        FAssignNewOwned(PropertyEditor, editor, transient);

        return editor;
    }

} // namespace CE

