#pragma once

namespace CE::Editor
{
	CLASS(Config = Editor)
	class EDITORWIDGETS_API FieldEditor : public CLayoutGroup
	{
		CE_CLASS(FieldEditor, CLayoutGroup)
	public:

		FieldEditor();
		virtual ~FieldEditor();

		static SubClassType<FieldEditor> GetFieldEditorClass(Name fieldDeclarationTypeName);

		virtual bool IsFieldTypeSupported(TypeInfo* type);

		bool SetTargets(TypeInfo* allFieldsDeclType, const Array<FieldType*>& fieldTypes, const Array<void*>& targetInstances);

		bool IsLayoutCalculationRoot() override { return true; }

		virtual bool IsExpandable();

	protected:

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void Construct() override;

		void OnDrawGUI() override;

		template<typename T>
		void SetTargetFieldValue(const T& value)
		{
			if (fieldTypes.GetSize() != targets.GetSize())
				return;

			TypeInfo* valueType = TYPE(T);
			if (valueType == nullptr || fieldDeclType == nullptr)
				return;
			
			for (int i = 0; i < fieldTypes.GetSize(); i++)
			{
				auto fieldDeclTypeId = fieldTypes[i]->GetDeclarationTypeId();
				if (fieldDeclTypeId == TYPEID(T) || (valueType != nullptr && valueType->IsAssignableTo(fieldDeclTypeId)))
					fieldTypes[i]->SetFieldValue(targets[i], value);
			}
		}

	editorwidgets_protected_internal:

		static HashMap<Name, Array<ClassType*>> editorClassesByFieldType;

		TypeInfo* fieldDeclType = nullptr;
		Array<FieldType*> fieldTypes{};
		Array<void*> targets{};
		CInputValidator textValidator = nullptr;

		FIELD(Config)
		bool useDragSlider = false;
	};
    
} // namespace CE::Editor

#include "FieldEditor.rtti.h"
