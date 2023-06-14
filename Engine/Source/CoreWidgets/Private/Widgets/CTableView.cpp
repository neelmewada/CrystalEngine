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

		GUI::TableFlags tableFlags{};
		if (HasAnyTableFlags(CTableFlags::ResizeableColumns))
			tableFlags |= GUI::TableFlags_Resizable;
		if (HasAnyTableFlags(CTableFlags::ReorderableColumns))
			tableFlags |= GUI::TableFlags_Reorderable;

		if (model == nullptr)
		{
			if (GUI::BeginTable(id, 0, tableFlags))
			{
				GUI::EndTable();
				PollEvents();
			}
		}
		else
		{
			if (GUI::BeginTable(id, model->GetColumnCount(CModelIndex()), tableFlags))
			{
				if (model->HasHeader())
				{
					for (int c = 0; c < model->GetColumnCount({}); c++)
					{
						GUI::TableSetupColumn(model->GetHeaderTitle(c));
					}
					GUI::TableHeadersRow();
				}

				DrawTableContents({});

				GUI::EndTable();
				PollEvents();
			}
		}

		style.Pop();
	}

	void CTableView::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

	void CTableView::DrawTableContents(const CModelIndex& parent)
	{
		if (model == nullptr)
			return;

		for (int r = 0; r < model->GetRowCount(parent); r++)
		{
			GUI::TableNextRow();
			if (model->GetColumnCount(parent) == 0)
				continue;

			u32 childRowCount = model->GetRowCount(model->GetIndex(r, 0, parent));
			bool open = false;

			for (int c = 0; c < model->GetColumnCount(parent); c++)
			{
				CModelIndex cellIndex = model->GetIndex(r, c, parent);

				GUI::TableNextColumn();
				
				if (c == 0 && childRowCount > 0)
				{
					open = GUI::TreeNode(model->GetText(cellIndex));
					continue;
				}

				CWidget* widget = nullptr;

				if (!widgetMap.KeyExists(cellIndex))
				{
					ClassType* widgetClass = model->GetWidgetClass(cellIndex);
					if (widgetClass == nullptr || !widgetClass->IsSubclassOf<CWidget>() || !widgetClass->CanBeInstantiated())
						continue;

					auto& widgetList = freeWidgetMap[widgetClass->GetTypeId()];
					if (widgetList.IsEmpty())
						widgetList.Add(CreateWidget<CWidget>(this, String::Format("Cell:{},{}", 12, 12), widgetClass));

					widget = widgetList.Top();
					widgetList.Pop();
					widgetMap[cellIndex] = widget;
				}

				if (widget == nullptr)
					widget = widgetMap[cellIndex];
				
				model->SetData(cellIndex, widget);

				widget->RenderGUI(); // Render widget
			}

			if (open)
			{
				DrawTableContents(model->GetIndex(r, 0, parent));
				GUI::TreePop();
			}
		}
	}

} // namespace CE::Widgets
