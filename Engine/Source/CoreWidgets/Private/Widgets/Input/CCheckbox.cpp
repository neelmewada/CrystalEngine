#include "CoreWidgets.h"

namespace CE::Widgets
{
    CCheckbox::CCheckbox()
    {

    }

    CCheckbox::~CCheckbox()
    {

    }

	Vec2 CCheckbox::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(defaultStyleState.fontSize, defaultStyleState.fontSize);
	}

	void CCheckbox::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();

		if (GUI::Checkbox(rect, GetUuid(), &tristateValue, defaultStyleState))
		{
			emit OnValueChanged(tristateValue);
		}
		PollEvents();
    }

} // namespace CE::Widgets
