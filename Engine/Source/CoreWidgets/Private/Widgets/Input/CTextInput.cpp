#include "CoreWidgets.h"

namespace CE::Widgets
{
	CTextInput::CTextInput()
	{

	}

	CTextInput::~CTextInput()
	{
	}

	Vec2 CTextInput::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(100, 18);
	}

	void CTextInput::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

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

		if (id == 0)
			id = UUID32();

		auto inputChanged = GUI::InputText(rect, (u32)id, hint, value, defaultStyleState, defaultStyleState, defaultStyleState, padding, flags, callback, this);
		PollEvents();

		if (inputChanged)
		{
			emit OnTextEdited(value);
		}
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
