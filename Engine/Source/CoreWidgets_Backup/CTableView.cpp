#include "CoreWidgets.h"

namespace CE::Widgets
{

	void CTableView::OnDrawGUI()
	{
		style.Push();

		if (style.IsDirty())
		{
			Vec2 availSpace = GUI::GetContentRegionAvailableSpace();

			auto& widthProperty = style.GetProperty(CStylePropertyType::Width);
			if (widthProperty.IsValid())
			{
				tableSize.x = Math::Min(widthProperty.single * (widthProperty.isPercent ? availSpace.x / 100.0f : 1), availSpace.x);
			}
			auto& heightProperty = style.GetProperty(CStylePropertyType::Height);
			if (heightProperty.IsValid())
			{
				tableSize.y = Math::Min(heightProperty.single * (heightProperty.isPercent ? availSpace.y / 100.0f : 1), availSpace.y);
			}

			auto& maxWidthProperty = style.GetProperty(CStylePropertyType::MaxWidth);
			if (maxWidthProperty.IsValid())
			{
				tableSize.x = Math::Min(maxWidthProperty.single * (maxWidthProperty.isPercent ? availSpace.x / 100.0f : 1), tableSize.x);
			}

			auto& maxHeightProperty = style.GetProperty(CStylePropertyType::MaxHeight);
			if (maxHeightProperty.IsValid())
			{
				tableSize.y = Math::Min(maxHeightProperty.single * (maxHeightProperty.isPercent ? availSpace.y / 100.0f : 1), tableSize.y);
			}

			style.SetDirty(false);
		}

		if (id.IsEmpty())
		{
			id = String::Format("{}", GetUuid());
		}

		GUI::TableFlags tableFlags{};
		if (HasAnyTableFlags(CTableFlags::ResizeableColumns))
			tableFlags |= GUI::TableFlags_Resizable;
		if (HasAnyTableFlags(CTableFlags::ReorderableColumns))
			tableFlags |= GUI::TableFlags_Reorderable;
		if (HasAnyTableFlags(CTableFlags::RowBackground))
			tableFlags |= GUI::TableFlags_RowBg;
		if (HasAnyTableFlags(CTableFlags::ScrollX))
			tableFlags |= GUI::TableFlags_ScrollX;
		if (HasAnyTableFlags(CTableFlags::ScrollY))
			tableFlags |= GUI::TableFlags_ScrollY;
		if (HasAnyTableFlags(CTableFlags::BordersInnerH))
			tableFlags |= GUI::TableFlags_BordersInnerH;
		if (HasAnyTableFlags(CTableFlags::BordersInnerV))
			tableFlags |= GUI::TableFlags_BordersInnerV;
		if (HasAnyTableFlags(CTableFlags::BordersOuterH))
			tableFlags |= GUI::TableFlags_BordersOuterH;
		if (HasAnyTableFlags(CTableFlags::BordersOuterV))
			tableFlags |= GUI::TableFlags_BordersOuterV;

		if (model == nullptr)
		{
			if (GUI::BeginTable(id, 0, tableFlags, tableSize))
			{
				GUI::EndTable();
				PollEvents();
			}
		}
		else
		{
			if (GUI::BeginTable(id, model->GetColumnCount(CModelIndex()), tableFlags, tableSize))
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

			auto rect = GUI::GetTableClipRect();
			auto curPos = GUI::GetCursorScreenPos();
			bool clipped = false;

			if (rect.w < 1'000'000'000'000.0f && (curPos.y < rect.y - 60 || curPos.y > rect.w + 60))
			{
				clipped = true;
			}

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
				ClassType* widgetClass = model->GetWidgetClass(cellIndex);

				if (clipped)
				{
					if (widgetMap.KeyExists(cellIndex))
					{
						widget = widgetMap[cellIndex];
						widgetMap.Remove(cellIndex);

						auto& widgetList = freeWidgetMap[widgetClass->GetTypeId()];
						if (widget != nullptr)
							widgetList.Add(widget);
					}

					GUI::Text("");
					continue;
				}

				if (!widgetMap.KeyExists(cellIndex))
				{
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
                
                int indentLevel = model->GetIndentLevel(cellIndex);
                
                for (int i = 0; i < indentLevel; i++)
                {
                    GUI::Indent();
                }

				widget->RenderGUI(); // Render widget
                
                for (int i = 0; i < indentLevel; i++)
                {
                    GUI::Unindent();
                }
			}

			if (open)
			{
				DrawTableContents(model->GetIndex(r, 0, parent));
				GUI::TreePop();
			}
		}
	}

} // namespace CE::Widgets
