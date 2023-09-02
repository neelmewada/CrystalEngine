#include "EditorWidgets.h"

namespace CE::Editor
{
	HashMap<Name, Array<ClassType*>> FieldEditor::editorClassesByFieldType{};

    FieldEditor::FieldEditor()
    {

    }

    FieldEditor::~FieldEditor()
    {

    }

	SubClassType<FieldEditor> FieldEditor::GetFieldEditorClass(Name fieldDeclarationTypeName)
	{
		TypeInfo* fieldDeclType = GetTypeInfo(fieldDeclarationTypeName);

		if (fieldDeclType->IsClass())
		{
			ClassType* classType = (ClassType*)fieldDeclType;
			ClassType* baseClass = classType;

			while (baseClass != nullptr)
			{
				if (editorClassesByFieldType.KeyExists(baseClass->GetTypeName()))
				{
					const auto& array = editorClassesByFieldType[baseClass->GetTypeName()];
					if (array.NonEmpty())
						return array.Top();
				}

				if (baseClass->GetSuperClassCount() > 0)
					baseClass = baseClass->GetSuperClass(0);
				else break;
			}
		}
		else if (fieldDeclType->IsStruct())
		{
			StructType* structType = (StructType*)fieldDeclType;
			
			if (editorClassesByFieldType.KeyExists(structType->GetTypeName()))
			{
				const auto& array = editorClassesByFieldType[structType->GetTypeName()];
				if (array.NonEmpty())
					return array.Top();
			}
		}
		else if (editorClassesByFieldType.KeyExists(fieldDeclarationTypeName))
		{
			return editorClassesByFieldType[fieldDeclarationTypeName].Top();
		}

		return FieldEditor::StaticType();
	}

	bool FieldEditor::IsFieldTypeSupported(FieldType* fieldType)
	{
		if (fieldType == nullptr)
			return false;

		auto type = fieldType->GetDeclarationType();
		auto typeId = fieldType->GetDeclarationTypeId();

		if (fieldType->IsIntegerField() ||
			fieldType->IsDecimalField())
		{
			return true;
		}
		else if (type->IsStruct())
		{
			return true;
		}
		else if (type->IsObject())
		{
			return true;
		}

		static const Array<TypeId> allowedTypeIds = {
			TYPEID(Vec2), TYPEID(Vec3), TYPEID(Vec4),
			TYPEID(Vec2i), TYPEID(Vec3i), TYPEID(Vec4i),
			TYPEID(Color), TYPEID(String), TYPEID(Name), 
			TYPEID(UUID), TYPEID(UUID32)
		};

		return false;
	}

	void FieldEditor::Construct()
	{
		Super::Construct();


	}

} // namespace CE::Editor
