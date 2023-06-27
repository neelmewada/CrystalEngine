#include "CoreWidgets.h"

namespace CE::Widgets
{

	CCheckbox::CCheckbox()
	{

	}

	CCheckbox::~CCheckbox()
	{

	}

	void CCheckbox::OnDrawGUI()
	{
		style.Push();

		bool hasPadding = false;
		Vec4 padding{};
		Vec4 borderRadius{};
		CStyleValue* background = nullptr;
		CStyleValue* backgroundHovered = nullptr;
		CStyleValue* backgroundPressed = nullptr;
		bool hasCheckColor = false;
		Color checkColor{};
		f32 borderThickness = 0.0f;

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::None)
				{
					hasPadding = true;
					padding = styleValue.vector;
				}
				else if (property == CStylePropertyType::Background && styleValue.subControl == CSubControl::None)
				{
					if ((styleValue.state & CStateFlag::Hovered) != 0)
					{
						backgroundHovered = &styleValue;
					}
					if ((styleValue.state & CStateFlag::Pressed) != 0)
					{
						backgroundPressed = &styleValue;
					}
					if (styleValue.state == CStateFlag::Default)
					{
						background = &styleValue;
					}
				}
				else if (property == CStylePropertyType::BorderRadius && styleValue.state == CStateFlag::Default && styleValue.subControl == CSubControl::None)
				{
					if (styleValue.valueType == CStyleValue::Type_Vector)
						borderRadius = styleValue.vector;
					else if (styleValue.valueType == CStyleValue::Type_Single)
						borderRadius = Vec4(1, 1, 1, 1) * styleValue.single;
				}
				else if (property == CStylePropertyType::BorderWidth && styleValue.valueType == CStyleValue::Type_Single)
				{
					borderThickness = styleValue.single;
				}
			}
		}

		if (!hasPadding)
		{
			padding = GUI::GetDefaultPadding();
		}

		s8 curValue = value;
		GUI::CheckboxTriState(GetUuid(), &curValue, padding, borderRadius, borderThickness);
		if (value != curValue)
		{
			value = curValue;
			emit OnValueChanged(value);
		}

		PollEvents();

		style.Pop();
	}

	void CCheckbox::HandleEvent(CEvent* event)
	{


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
