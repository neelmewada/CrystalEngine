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

        Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

        // Getters & Setters

        const CInputValidator& GetInputValidator() const
        {
            return validator;
        }

        void SetInputValidator(CInputValidator validator)
        {
            this->validator = validator;
        }

        inline void SetText(const String& text)
        {
            this->value = text;
        }

        inline const String& GetText() const { return value; }

        inline void SetHint(const String& hint) { this->hint = hint; }
        inline const String& GetHint() const { return hint; }

    public: // Signals

        CE_SIGNAL(OnTextEdited, String);

    protected:

		void OnAfterComputeStyle() override;

        void OnDrawGUI() override;

        void HandleEvent(CEvent* event) override;


    protected:

        FIELD()
		String value{};

		FIELD()
		String hint{};

		FIELD()
		b8 isPassword = false;

		FIELD()
		b8 isMultiline = false;

		UUID32 id = 0;

		Color hintColor = Color::White();

        CInputValidator validator{};
    };

} // namespace CE::Widgets

#include "CTextInput.rtti.h"
