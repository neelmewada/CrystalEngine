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

	void CTextInput::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		auto matchStyle = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Hint);
		if (matchStyle.IsValidProperty(CStylePropertyType::Foreground, CStyleValue::Type_Color))
		{
			hintColor = matchStyle.properties[CStylePropertyType::Foreground].color;
		}
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

		auto styleState = defaultStyleState;
		styleState.background = {};
		styleState.borderThickness = 0;
		styleState.borderColor = {};

		auto inputChanged = GUI::InputText(rect, (u32)id, hint, value, styleState, hintColor, padding, flags, callback, this);
		PollEvents();

		if (inputChanged)
		{
			emit OnTextEdited(value);
		}
	}

	void CTextInput::HandleEvent(CEvent* event)
	{
		if (event->type == CEventType::FocusChanged)
		{
			CFocusEvent* focusEvent = (CFocusEvent*)event;
			if (focusEvent->GotFocus())
			{
				stateFlags = CStateFlag::Active;
			}
			else // Lost focus
			{
				stateFlags = CStateFlag::Default;
			}
			emit OnFocusChanged(focusEvent->GotFocus());
			SetNeedsStyle();
			event->MarkHandled();
		}

		Super::HandleEvent(event);
	}

	COREWIDGETS_API u16 CNameInputValidator(const String& input, u16 appendChar, int cursorPos)
	{
		if (String::IsAlphabet(appendChar) || String::IsNumeric(appendChar) || appendChar == '-' || appendChar == '_')
			return appendChar;
		return 0;
	}

	COREWIDGETS_API u16 CFloatInputValidator(const String& input, u16 appendChar, int cursorPos)
	{
		if (String::IsNumeric(appendChar))
		{
			for (int i = cursorPos; i < input.GetLength(); i++)
			{
				if (input[i] == '-')
					return 0;
			}
			return appendChar;
		}

		switch (appendChar)
		{
		case '-':
			if (input.IsEmpty())
				return appendChar;
			if (cursorPos == 0 && !input.Contains('-'))
				return appendChar;
			break;
		case '.':
			if (!input.Contains('.'))
				return appendChar;
			break;
		}

		return 0;
	}

	COREWIDGETS_API u16 CIntegerInputValidator(const String& input, u16 appendChar, int cursorPos)
	{
		if (String::IsNumeric(appendChar))
		{
			for (int i = cursorPos; i < input.GetLength(); i++)
			{
				if (input[i] == '-')
					return 0;
			}
			return appendChar;
		}

		switch (appendChar)
		{
		case '-':
			if (input.IsEmpty())
				return appendChar;
			if (cursorPos == 0 && !input.Contains('-'))
				return appendChar;
			break;
		}

		return 0;
	}

	COREWIDGETS_API u16 CUnsignedIntegerInputValidator(const String& input, u16 appendChar, int cursorPos)
	{
		if (String::IsNumeric(appendChar))
		{
			return appendChar;
		}

		return 0;
	}

    COREWIDGETS_API u16 CLongVersionInputValidator(const String& input, u16 appendChar, int cursorPos)
    {
		if (appendChar == '.')
		{
			int totalPeriods = 0;
			for (int i = 0; i < input.GetLength(); i++)
			{
				if (input[i] == '.')
					totalPeriods++;
			}
			if (totalPeriods >= 3)
				return 0;
			if (cursorPos <= 0 || !String::IsNumeric(input[cursorPos - 1]))
				return 0;
			return appendChar;
		}
		else if (String::IsNumeric(appendChar))
		{
			return appendChar;
		}

		return 0;
    }

	COREWIDGETS_API u16 CVersionInputValidator(const String& input, u16 appendChar, int cursorPos)
	{
		if (appendChar == '.')
		{
			int totalPeriods = 0;
			for (int i = 0; i < input.GetLength(); i++)
			{
				if (input[i] == '.')
					totalPeriods++;
			}
			if (totalPeriods >= 2)
				return 0;
			if (cursorPos <= 0 || !String::IsNumeric(input[cursorPos - 1]))
				return 0;
			return appendChar;
		}
		else if (String::IsNumeric(appendChar))
		{
			return appendChar;
		}

		return 0;
	}

} // namespace CE::Widgets
