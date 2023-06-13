#include "CoreWidgets.h"

namespace CE::Widgets
{

	void CTableView::OnDrawGUI()
	{
		style.Push();

		if (id.IsEmpty())
		{
			id = String::Format("{}", GetUuid());
		}

		if (model == nullptr)
		{
			if (GUI::BeginTable(id, 0))
			{
				GUI::EndTable();
			}
		}
		else
		{
			if (GUI::BeginTable(id, model->GetColumnCount(CModelIndex())))
			{
				for (int r = 0; r < model->GetRowCount({}); r++)
				{
					for (int c = 0; c < model->GetColumnCount({}); c++)
					{

					}
				}

				GUI::EndTable();
			}
		}

		style.Pop();
	}

	void CTableView::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
