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

		bool hasPadding = false;
		if (style.styleMap.KeyExists(CStyleProperty::Padding))
		{
			padding = style.styleMap[CStyleProperty::Padding].vector;
		}

		if (padding.x > 0.1f || padding.y > 0.1f || padding.z > 0.1f || padding.w > 0.1f)
			hasPadding = true;

		if (GUI::Button(text, padding))
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
