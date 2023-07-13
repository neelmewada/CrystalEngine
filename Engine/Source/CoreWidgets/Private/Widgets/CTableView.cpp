#include "CoreWidgets.h"

namespace CE::Widgets
{
	CAbstractItemCell::CAbstractItemCell()
	{

	}

	CAbstractItemCell::~CAbstractItemCell()
	{

	}

	Vec2 CAbstractItemCell::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return GUI::CalculateTextSize(text) + Vec2(6, 3);
	}

	void CAbstractItemCell::OnDrawGUI()
	{
		Vec2 cursorPos = GUI::GetCursorPos();
		Rect rect = GetComputedLayoutRect();
		Vec2 size = GetComputedLayoutSize();
		
		Rect localRect = Rect(cursorPos, cursorPos + size);
		
		GUI::Text(text);
	}

    CTableView::CTableView()
    {
        
    }

    CTableView::~CTableView()
    {
        
    }

	Vec2 CTableView::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(width, height);
	}

	void CTableView::SetCellHeight(f32 cellHeight)
	{
		this->cellHeight = cellHeight;
		SetNeedsStyle();
		SetNeedsLayout();
	}

    void CTableView::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		auto rectSize = rect.max - rect.min;

		DrawDefaultBackground();

		GUI::TableFlags tableFlags{};
		if (HasAnyTableFlags(CTableFlags::ResizeableColumns))
			tableFlags |= GUI::TableFlags_Resizable;
		if (HasAnyTableFlags(CTableFlags::ReorderableColumns))
			tableFlags |= GUI::TableFlags_Reorderable;
		if (HasAnyTableFlags(CTableFlags::ScrollX))
			tableFlags |= GUI::TableFlags_ScrollX;
		if (HasAnyTableFlags(CTableFlags::ScrollY))
			tableFlags |= GUI::TableFlags_ScrollY;

		if (model == nullptr)
		{
			if (GUI::BeginTable(rect, GetUuid(), GetName().GetString(), 0, tableFlags))
			{
				GUI::EndTable();
				PollEvents();
			}
		}
		else
		{
			if (GUI::BeginTable(rect, GetUuid(), GetName().GetString(), model->GetColumnCount(CModelIndex()), tableFlags))
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

				auto pos = GUI::GetCursorPos();

				if (c == 0 && childRowCount > 0)
				{
					GUI::ID cellId = (GUI::ID)GetCombinedHashes({ GetUuid(), (SIZE_T)r, (SIZE_T)c });
					open = GUI::TableTreeNode(cellId);
					GUI::SetCursorPos(pos);
					//continue;
				}

				CAbstractItemCell* widget = nullptr;
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
					if (widgetClass == nullptr || !widgetClass->IsSubclassOf<CAbstractItemCell>() || !widgetClass->CanBeInstantiated())
						continue;

					auto& widgetList = freeWidgetMap[widgetClass->GetTypeId()];
					if (widgetList.IsEmpty())
						widgetList.Add(CreateWidget<CAbstractItemCell>(this, "TableCell", widgetClass));

					widget = widgetList.Top();
					widgetList.Pop();
					widgetMap[cellIndex] = widget;
				}

				if (widget == nullptr)
					widget = widgetMap[cellIndex];

				model->SetData(cellIndex, widget);

				int indentLevel = model->GetIndentLevel(cellIndex);
				f32 indentAmount = 0;
				if (open) indentAmount = 0.1f;

				for (int i = 0; i < indentLevel; i++)
				{
					GUI::Indent(indentAmount);
				}

				widget->RenderGUI(); // Render widget

				for (int i = 0; i < indentLevel; i++)
				{
					GUI::Unindent(indentAmount);
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
