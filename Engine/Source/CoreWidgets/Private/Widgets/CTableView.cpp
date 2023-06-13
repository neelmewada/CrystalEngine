#include "CoreWidgets.h"

namespace CE::Widgets
{

	void CTableView::OnDrawGUI()
	{
		style.Push();


		style.Pop();
	}

	void CTableView::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
