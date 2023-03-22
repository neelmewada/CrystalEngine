
#include "Drawers/VectorFieldDrawer.h"

#include <QLabel>

namespace CE::Editor
{

	VectorFieldDrawer::VectorFieldDrawer() : FieldDrawer("VectorFieldDrawer")
	{
		
	}

	VectorFieldDrawer::~VectorFieldDrawer()
	{

	}

	void VectorFieldDrawer::OnValuesUpdated()
	{
		auto typeId = fieldType->GetDeclarationType()->GetTypeId();
		if (targetInstance == nullptr || vec4Field == nullptr)
			return;

		if (typeId == TYPEID(Vec4))
		{
			Vec4 value = fieldType->GetFieldValue<Vec4>(targetInstance);
			vec4Field->SetValue(value);
		}
		else if (typeId == TYPEID(Vec3))
		{
			Vec4 value = fieldType->GetFieldValue<Vec3>(targetInstance);
			value.w = 0;
			vec4Field->SetValue(value);
		}
		else if (typeId == TYPEID(Vec2))
		{
			Vec4 value = fieldType->GetFieldValue<Vec2>(targetInstance);
			value.z = value.w = 0;
			vec4Field->SetValue(value);
		}
		else if (typeId == TYPEID(Vec4i))
		{
			Vec4i value = fieldType->GetFieldValue<Vec4i>(targetInstance);
			vec4Field->SetValue(Vec4i{ value.x, value.y, value.z, value.w });
		}
		else if (typeId == TYPEID(Vec3i))
		{
			Vec4i value = fieldType->GetFieldValue<Vec3i>(targetInstance);
			vec4Field->SetValue(Vec4i{ value.x, value.y, value.z, 0 });
		}
		else if (typeId == TYPEID(Vec2i))
		{
			Vec4i value = fieldType->GetFieldValue<Vec2i>(targetInstance);
			vec4Field->SetValue(Vec4i{ value.x, value.y, 0, 0 });
		}
	}

	void VectorFieldDrawer::CreateGUI(QLayout* container)
	{
		if (!IsValid())
			return;

		if (vec4Field != nullptr)
		{
			delete vec4Field;
		}

		vec4Field = new Qt::Vec4Input(container->parentWidget());
        
        auto result = vec4Field->Bind("OnInputValueChanged", this, "OnValueChanged");

		Qt::Vec4Mode mode{};
		bool isInteger = false;

		auto typeId = fieldType->GetDeclarationType()->GetTypeId();

		if (typeId == TYPEID(Vec4))
		{
			mode = Qt::Vec4Mode::Vec4;
		}
		else if (typeId == TYPEID(Vec3))
		{
			mode = Qt::Vec4Mode::Vec3;
		}
		else if (typeId == TYPEID(Vec2))
		{
			mode = Qt::Vec4Mode::Vec2;
		}
		else if (typeId == TYPEID(Vec4i))
		{
			mode = Qt::Vec4Mode::Vec4;
			isInteger = true;
		}
		else if (typeId == TYPEID(Vec3i))
		{
			mode = Qt::Vec4Mode::Vec3;
			isInteger = true;
		}
		else if (typeId == TYPEID(Vec2i))
		{
			mode = Qt::Vec4Mode::Vec2;
			isInteger = true;
		}

		vec4Field->SetMode(mode, isInteger);
		vec4Field->SetLabel(fieldType->GetDisplayName());

		container->addWidget(vec4Field);

		OnValuesUpdated();
	}

	void VectorFieldDrawer::OnValueChanged(Vec4 value)
	{
		auto typeId = fieldType->GetDeclarationType()->GetTypeId();
		if (targetInstance == nullptr || vec4Field == nullptr)
			return;

		if (typeId == TYPEID(Vec4))
		{
			fieldType->SetFieldValue<Vec4>(targetInstance, value);
		}
		else if (typeId == TYPEID(Vec3))
		{
			fieldType->SetFieldValue<Vec3>(targetInstance, value);
		}
		else if (typeId == TYPEID(Vec2))
		{
			fieldType->SetFieldValue<Vec2>(targetInstance, value);
		}
		else if (typeId == TYPEID(Vec4i))
		{
			fieldType->SetFieldValue<Vec4i>(targetInstance, Vec4i{ (s32)value.x, (s32)value.y, (s32)value.z, (s32)value.w });
		}
		else if (typeId == TYPEID(Vec3i))
		{
			fieldType->SetFieldValue<Vec3i>(targetInstance, Vec3i{ (s32)value.x, (s32)value.y, (s32)value.z });
		}
		else if (typeId == TYPEID(Vec2i))
		{
			fieldType->SetFieldValue<Vec2i>(targetInstance, Vec2i{ (s32)value.x, (s32)value.y });
		}
	}

} // namespace CE::Editor

CE_RTTI_CLASS_IMPL(EDITORCORE_API, CE::Editor, VectorFieldDrawer)
