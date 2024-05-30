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

		bool SetText(const String& value);

		const String& GetText() const { return text; }

		const String& GetHint() const { return hint; }

		void SetHint(const String& hint);

		b8 IsPassword() const { return isPassword; }

		void SetAsPassword(b8 set);

		b8 IsEditing() const { return isEditing; }

		void StopEditing(bool restoreOriginalText = false);

		void StartEditing();

		b8 IsEditable() const { return isEditable; }

		void SetEditable(bool editable) { isEditable = editable; }

		void SetInputValidator(CInputValidator validator)
		{
			this->inputValidator = validator;
		}

		bool IsTextSelected();

		void SetSelectAllOnEdit(bool set) { selectAllOnEdit = set; }

		// - Signals -

		CE_SIGNAL(OnEditingFinished, CTextInput*);

		// Called when text is modified through user input!
		CE_SIGNAL(OnTextEdited, CTextInput*);

		// Called when text is modified through either user input or through code!
		CE_SIGNAL(OnTextChanged, CTextInput*);

	protected:

		virtual void OnValidateText() {}

		bool SetTextInternal(const String& value);

		FUNCTION()
		void OnTimerTick();

		// For internal use only!
		String GetDisplayText();

		Vec2 GetComputedLayoutSize() override;

		void OnPaint(CPaintEvent* paintEvent) override;

		void HandleEvent(CEvent* event) override;

		void OnFocusGained() override;

		void OnFocusLost() override;

		void OnDisabled() override;

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
		b8 selectAllOnEdit = false;

		FIELD()
		CTimer* timer = nullptr;

	protected:

		bool PostComputeStyle() override
		{
			return RecalculateOffsets();
		}

		// - Internal API -

		bool RecalculateOffsets();

		void SetCursorPos(int cursorPos);

		void ScrollTo(int cursorPos);

		void InsertAt(const String& string, int insertPos);

		void RemoveRange(int startIndex, int count);

		void RemoveSelected();

		void SelectRange(int startIndex, int endIndex);
		void SelectAll();

		String GetSelectedText();

		void DeselectAll();

		// Fields

		CInputValidator inputValidator{};

		String originalText{};

		f32 textScrollOffset = 0;

		Array<Rect> characterOffsets{};
		Vec2 textSize{};

		int cursorPos = 0;
		b8 isSelectionActive = false;
		RangeInt selectionRange{};

		b8 cursorState = true;
		b8 isCursorMoving = false;

		f32 selectionDistance = 0;

		b8 isEditing = false;

	};

} // namespace CE::Widgets

#include "CTextInput.rtti.h"