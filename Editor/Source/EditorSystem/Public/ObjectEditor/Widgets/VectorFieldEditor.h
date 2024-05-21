#pragma once

namespace CE::Editor
{
	CLASS()
	class EDITORSYSTEM_API VectorComponentInput : public CTextInput
	{
		CE_CLASS(VectorComponentInput, CTextInput)
	public:


	protected:

		void OnFocusLost() override;

		void Construct() override;

		void OnPaintOverlay(CPaintEvent* paintEvent) override;

		FIELD()
		Color tagColor = Color::Clear();

		friend class VectorFieldEditor;
	};

	CLASS()
	class EDITORSYSTEM_API VectorFieldEditor : public FieldEditor
	{
		CE_CLASS(VectorFieldEditor, FieldEditor)
	public:

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
			else
			{
				fieldZ->SetEnabled(false);
				fieldW->SetEnabled(false);
			}

			if constexpr (TContainsType<T, Vec2, Vec3, Vec4>::Value)
			{
				fieldX->SetInputValidator(CFloatInputValidator);
				fieldY->SetInputValidator(CFloatInputValidator);
				fieldZ->SetInputValidator(CFloatInputValidator);
				fieldW->SetInputValidator(CFloatInputValidator);
			}
			else if constexpr (TContainsType<T, Vec2i, Vec3i, Vec4i>::Value)
			{
				fieldX->SetInputValidator(CIntegerInputValidator);
				fieldY->SetInputValidator(CIntegerInputValidator);
				fieldZ->SetInputValidator(CIntegerInputValidator);
				fieldW->SetInputValidator(CIntegerInputValidator);
			}
		}

		void SetVectorType(TypeId vectorTypeId);

		Vec4 GetVectorValue();

		Vec4i GetVectorIntValue();

		void SetVectorValue(const Vec4& value);
		void SetVectorIntValue(const Vec4i& value);

		void BindField(FieldType* field, void* instance) override;

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

		FieldType* field = nullptr;
		void* instance = nullptr;

		TypeId vectorType = 0;
	};

} // namespace CE::Editor

#include "VectorFieldEditor.rtti.h"