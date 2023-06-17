#include "CoreWidgets.h"

namespace CE::Widgets
{
	CTextInput::CTextInput()
	{
		id = (u64)GetUuid();
	}

	CTextInput::~CTextInput()
	{

	}

	void CTextInput::OnDrawGUI()
	{
		style.Push();

		Vec4 padding{ 5, 5, 5, 5 };
		Vec4 borderRadius{};
		Vec2 preferredSize = {};
		Vec2 maxSize = {};
		Vec2 minSize = {};
		f32 borderThickness = 1.0f;

		for (auto& [property, array] : style.styleMap)
		{
			for (auto& styleValue : array)
			{
				if (property == CStylePropertyType::Padding)
				{
					if (styleValue.state == CStateFlag::Default)
						padding = styleValue.vector;
				}
				else if (property == CStylePropertyType::BorderRadius)
				{
					borderRadius = styleValue.vector;
				}
				else if (property == CStylePropertyType::Width)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							preferredSize.x = styleValue.single;
						else
							preferredSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::Height)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							preferredSize.y = styleValue.single;
						else
							preferredSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
				else if (property == CStylePropertyType::MaxWidth)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							maxSize.x = styleValue.single;
						else
							maxSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::MaxHeight)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							maxSize.y = styleValue.single;
						else
							maxSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
				else if (property == CStylePropertyType::MinWidth)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							minSize.x = styleValue.single;
						else
							minSize.x = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().x;
					}
				}
				else if (property == CStylePropertyType::MinHeight)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						if (!styleValue.isPercent)
							minSize.y = styleValue.single;
						else
							minSize.y = styleValue.single / 100.0f * GUI::GetContentRegionAvailableSpace().y;
					}
				}
				else if (property == CStylePropertyType::BorderWidth)
				{
					if (styleValue.state == CStateFlag::Default)
					{
						borderThickness = styleValue.single;
					}
				}
			}
		}

		GUI::TextInputFlags flags = GUI::TextInputFlags_EnterReturnsTrue | GUI::TextInputFlags_CallbackResize | GUI::TextInputFlags_CallbackCharFilter;
		if (isPassword)
			flags |= GUI::TextInputFlags_Password;
		if (isMultiline)
			flags |= GUI::TextInputFlags_Multiline;

		GUI::TextInputCallback callback = [](GUI::TextInputCallbackData* data) -> int
		{
			auto thisPtr = (CTextInput*)data->userData;
			if (data->eventFlag == GUI::TextInputFlags_CallbackCharFilter && thisPtr->GetInputValidator().IsValid())
			{
				data->eventChar = thisPtr->GetInputValidator().Invoke(thisPtr->value, data->eventChar, data->cursorPos);
			}
			return 0;
		};

		bool inputChanged = GUI::InputTextEx(id, hint, value, preferredSize, padding, borderRadius, borderThickness, minSize, maxSize, flags, callback, this);
		PollEvents();
		
		if (inputChanged)
		{
			emit OnValueChanged(value);
		}

		style.Pop();
	}

	void CTextInput::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

	COREWIDGETS_API u16 CFloatInputValidator(const String& input, u16 appendChar, int cursorPos)
	{
		if (String::IsNumeric(appendChar))
			return appendChar;
		if (appendChar == '-' && (input.IsEmpty() || cursorPos == 0))
			return appendChar;

		if (appendChar == '.')
		{
			for (int i = 0; i <= input.GetLength(); i++)
			{
				if (input[i] == '.')
					return 0;
			}
			return appendChar;
		}

		return 0;
	}

	COREWIDGETS_API u16 CIntegerInputValidator(const String& input, u16 appendChar, int cursorPos)
	{
		if (String::IsNumeric(appendChar))
			return appendChar;
		if (appendChar == '-' && (input.IsEmpty() || cursorPos == 0))
			return appendChar;

		return 0;
	}

} // namespace CE::Widgets
