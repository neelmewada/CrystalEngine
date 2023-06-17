#pragma once

namespace CE::Widgets
{
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
	};
    
} // namespace CE::Widgets


#include "CTextInput.rtti.h"