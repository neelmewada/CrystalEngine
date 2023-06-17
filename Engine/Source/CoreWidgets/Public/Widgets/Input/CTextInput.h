#pragma once

namespace CE::Widgets
{
	typedef Delegate<u16(const String& string, u16 appendChar, int cursorPos)> CInputValidator;

	COREWIDGETS_API u16 CFloatInputValidator(const String& input, u16 appendChar, int cursorPos);
	COREWIDGETS_API u16 CIntegerInputValidator(const String& input, u16 appendChar, int cursorPos);


	CLASS()
	class COREWIDGETS_API CTextInput : public CWidget
	{
		CE_CLASS(CTextInput, CWidget)
	public:

		CTextInput();
		virtual ~CTextInput();

		inline bool IsPassword() const
		{
			return isPassword;
		}

		inline void SetAsPassword(bool set)
		{
			isPassword = set;
		}

		inline const String& GetHint() const
		{
			return hint;
		}

		inline void SetHint(const String& hint)
		{
			this->hint = hint;
		}

		inline bool IsMultiline() const
		{
			return isMultiline;
		}

		inline void SetMultiline(bool set)
		{
			isMultiline = set;
		}

		const CInputValidator& GetInputValidator() const
		{
			return validator;
		}

		void SetInputValidator(CInputValidator validator)
		{
			this->validator = validator;
		}

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	public: // Signals

		CE_SIGNAL(OnValueChanged, String);

	private:

		FIELD()
		String value{};

		FIELD()
		String hint{};

		FIELD()
		b8 isPassword = false;

		FIELD()
		b8 isMultiline = false;

		u32 id = 0;

		CInputValidator validator{};
	};
    
} // namespace CE::Widgets


#include "CTextInput.rtti.h"