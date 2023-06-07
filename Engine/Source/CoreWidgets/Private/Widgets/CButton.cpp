#include "CoreWidgets.h"

namespace CE::Widgets
{

	CButton::CButton()
	{

	}

	CButton::~CButton()
	{

	}

	const String& CButton::GetText()
	{
		return text;
	}

	void CButton::SetText(const String& text)
	{
		this->text = text;
	}

	void CButton::OnDrawGUI()
	{
		PushStyle();
		
		Vec4 padding{};
		Vec4 borderRadius{};
		Alignment textAlignment = Alignment::Inherited;
		Vec2 preferredSize = {};

		if (style.styleMap.KeyExists(CStyleProperty::Padding))
		{
			padding = style.styleMap[CStyleProperty::Padding].vector;
		}

		if (style.styleMap.KeyExists(CStyleProperty::Size))
		{
			preferredSize = style.styleMap[CStyleProperty::Size].vector;
		}

		if (style.styleMap.KeyExists(CStyleProperty::TextAlignment))
		{
			textAlignment = (Alignment)style.styleMap[CStyleProperty::TextAlignment].enumValue;
		}

		if (style.styleMap.KeyExists(CStyleProperty::BorderRadius))
		{
			borderRadius = style.styleMap[CStyleProperty::BorderRadius].vector;
		}

		if (GUI::ButtonEx(text, padding, preferredSize, borderRadius, (GUI::TextAlign)textAlignment))
		{
			emit OnButtonClicked();
		}

		PopStyle();
	}

	void CButton::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
