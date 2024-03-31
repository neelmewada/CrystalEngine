#pragma once

namespace CE::Widgets
{
	CLASS(Config = Engine)
	class CRYSTALWIDGETS_API CTextInput : public CWidget
	{
		CE_CLASS(CTextInput, CWidget)
	public:

		CTextInput();
		virtual ~CTextInput();

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		void SetText(const String& value);

		const String& GetText() const { return text; }

		const String& GetHint() const { return hint; }

		void SetHint(const String& hint);

		b8 IsPassword() const { return isPassword; }

		void SetAsPassword(b8 set);

		b8 IsEditing() const { return isEditing; }

		b8 IsEditable() const { return isEditable; }

		void SetEditable(bool editable) { isEditable = editable; }

		void SelectAll() { selectAll = true; SetNeedsPaint(); }

		void SetInputValidator(CInputValidator validator)
		{
			this->inputValidator = validator;
		}

		bool IsTextSelected();

	protected:

		FUNCTION()
		void OnTimerTick();

		// For internal use only!
		String GetDisplayText();

		Vec2 GetComputedLayoutSize() override;

		void OnPaint(CPaintEvent* paintEvent) override;

		void HandleEvent(CEvent* event) override;

		void OnFocusGained() override;

		void OnFocusLost() override;

	crystalwidgets_protected_internal:

		FIELD()
		String text{};

		FIELD()
		String hint{};

		FIELD(Config)
		int cursorBlinkMillis = 500;

		FIELD()
		b8 isPassword = false;

		FIELD()
		b8 isMultiline = false;

		FIELD()
		b8 isEditable = true;

		FIELD()
		CTimer* timer = nullptr;

	private:

		// - Internal API -



		// Fields

		CInputValidator inputValidator{};

		String originalText{};

		f32 textScrollOffset = 0;

		int cursorPos = 0;
		RangeInt selectionRange{};

		b8 cursorState = true;
		b8 isCursorMoving = false;

		f32 selectionDistance = 0;

		b8 isEditing = false;

	};

} // namespace CE::Widgets

#include "CTextInput.rtti.h"