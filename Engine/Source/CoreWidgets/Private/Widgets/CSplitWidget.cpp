#include "CoreWidgets.h"

namespace CE::Widgets
{
    CSplitWidget::CSplitWidget()
    {

    }

    CSplitWidget::~CSplitWidget()
    {

    }

    void CSplitWidget::Construct()
    {
		Super::Construct();
    }

    void CSplitWidget::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		int count = attachedWidgets.GetSize();

		if (count <= 0)
			return;

		GUI::PushChildCoordinateSpace(rect);

		for (int i = 0; i < count; i++)
		{
			auto subWidget = attachedWidgets[i];

		}

		GUI::PopChildCoordinateSpace();
    }

} // namespace CE::Widgets
