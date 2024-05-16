#pragma once

namespace CE::Editor
{
	CLASS()
	class EDITORSYSTEM_API VectorComponentInput : public CWidget
	{
		CE_CLASS(VectorComponentInput, CWidget)
	public:


	protected:

		void Construct() override;

		FIELD()
		CWidget* tagWidget = nullptr;

		FIELD()
		CTextInput* inputField = nullptr;

		friend class VectorFieldEditor;
	};

	CLASS()
	class EDITORSYSTEM_API VectorFieldEditor : public FieldEditor
	{
		CE_CLASS(VectorFieldEditor, FieldEditor)
	public:

		VectorFieldEditor();

		virtual ~VectorFieldEditor();

		template<typename T> requires TContainsType<T, Vec2, Vec3, Vec4, Vec2i, Vec3i, Vec4i>::Value
		void SetVectorType()
		{
			vectorType = TYPEID(T);

			fieldX->SetEnabled(true);
			fieldY->SetEnabled(true);

			if constexpr (TContainsType<T, Vec3, Vec3i>::Value)
			{
				fieldZ->SetEnabled(true);
				fieldW->SetEnabled(false);
			}
			else if constexpr (TContainsType<T, Vec4, Vec4i>::Value)
			{
				fieldZ->SetEnabled(true);
				fieldW->SetEnabled(true);
			}

			if constexpr (TContainsType<T, Vec2, Vec3, Vec4>::Value)
			{
				fieldX->inputField->SetInputValidator(CFloatInputValidator);
				fieldY->inputField->SetInputValidator(CFloatInputValidator);
				fieldZ->inputField->SetInputValidator(CFloatInputValidator);
				fieldW->inputField->SetInputValidator(CFloatInputValidator);
			}
			else if constexpr (TContainsType<T, Vec2i, Vec3i, Vec4i>::Value)
			{
				fieldX->inputField->SetInputValidator(CIntegerInputValidator);
				fieldY->inputField->SetInputValidator(CIntegerInputValidator);
				fieldZ->inputField->SetInputValidator(CIntegerInputValidator);
				fieldW->inputField->SetInputValidator(CIntegerInputValidator);
			}
		}

		void SetVectorType(TypeId vectorTypeId);

		CE_SIGNAL(OnEditingFinished, VectorFieldEditor*);
		CE_SIGNAL(OnValueModified, VectorFieldEditor*);

		Vec4 GetVectorValue();

		Vec4i GetVectorIntValue();

	protected:

		void Construct() override;

		FIELD()
		VectorComponentInput* fieldX = nullptr;

		FIELD()
		VectorComponentInput* fieldY = nullptr;

		FIELD()
		VectorComponentInput* fieldZ = nullptr;

		FIELD()
		VectorComponentInput* fieldW = nullptr;

	private:

		TypeId vectorType = 0;
	};

} // namespace CE::Editor

#include "VectorFieldEditor.rtti.h"