#pragma once

namespace CE::Widgets
{
	CLASS()
	class CRYSTALWIDGETS_API CTextInput : public CWidget
	{
		CE_CLASS(CTextInput, CWidget)
	public:

		CTextInput();
		virtual ~CTextInput();

		Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

		void SetValue(const String& value);

		const String& GetValue() const { return value; }

		const String& GetHint() const { return hint; }

		void SetHint(const String& hint);

		b8 IsPassword() const { return isPassword; }

		void SetAsPassword(b8 set);

	protected:

		Vec2 GetComputedLayoutSize() override;

		void OnPaint(CPaintEvent* paintEvent) override;

		void HandleEvent(CEvent* event) override;

	crystalwidgets_protected_internal:

		FIELD()
		String value{};

		FIELD()
		String hint{};

		FIELD()
		b8 isPassword = false;

		FIELD()
		b8 isMultiline = false;

	};

} // namespace CE::Widgets

#include "CTextInput.rtti.h"