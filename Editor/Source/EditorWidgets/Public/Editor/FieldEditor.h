#pragma once

namespace CE::Editor
{
	CLASS()
	class EDITORWIDGETS_API FieldEditor : public CLayoutGroup
	{
		CE_CLASS(FieldEditor, CLayoutGroup)
	public:

		FieldEditor();
		virtual ~FieldEditor();

		static SubClassType<FieldEditor> GetFieldEditorClass(Name fieldDeclarationTypeName);

		virtual bool IsFieldTypeSupported(FieldType* fieldType);

	protected:

		void Construct() override;

	editorwidgets_protected_internal:

		static HashMap<Name, Array<ClassType*>> editorClassesByFieldType;
	};
    
} // namespace CE::Editor

#include "FieldEditor.rtti.h"
