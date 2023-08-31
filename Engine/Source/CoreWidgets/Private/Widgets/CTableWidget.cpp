#include "CoreWidgets.h"

namespace CE::Widgets
{
	CTableCellWidget::CTableCellWidget()
	{
	}

	CTableCellWidget::~CTableCellWidget()
	{
	}

	void CTableCellWidget::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		//GUI::PushChildCoordinateSpace(rect);

		for (auto widget : attachedWidgets)
		{
			widget->Render();
		}

		//GUI::PopChildCoordinateSpace();
	}

	CTableWidget::CTableWidget()
	{

	}

	CTableWidget::~CTableWidget()
	{

	}

	Vec2 CTableWidget::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		Vec2 intrinsicSize = Vec2(width, YGUndefined);
		f32 h = 0;
		for (int r = 0; r < numRows; r++)
		{
			for (int c = 0; c < numColumns; c++)
			{
				f32 maxHeight = 0;
				if (cellWidgets.KeyExists(Vec2i(r, c)))
				{
					auto cell = cellWidgets[Vec2i(r, c)];
					if (cell != nullptr)
					{
						cell->SetNeedsStyleRecursive();
						cell->SetNeedsLayoutRecursive();
						cell->UpdateLayoutIfNeeded();
						maxHeight = Math::Max(maxHeight, cell->GetComputedLayoutSize().height);
					}
				}
				h += maxHeight;
			}
		}
		if (!IsNan(intrinsicSize.height))
			intrinsicSize.height = h;
		return intrinsicSize;
	}

	void CTableWidget::CreateOrDestroyCellWidgets()
	{
		Array<Vec2i> cellsToRemove{};
		
		for (auto [pos, cell] : cellWidgets)
		{
			if (pos.x >= numRows || pos.y >= numColumns)
			{
				cellsToRemove.Add(pos);
			}
		}

		for (auto pos : cellsToRemove)
		{
			cellWidgets[pos]->Destroy();
			cellWidgets.Remove(pos);
		}

		for (int r = 0; r < numRows; r++)
		{
			for (int c = 0; c < numColumns; c++)
			{
				if (!cellWidgets.KeyExists(Vec2i(r, c)))
				{
					CTableCellWidget* cell = CreateWidget<CTableCellWidget>(this, "TableCell");
					cellWidgets[Vec2i(r, c)] = cell;
					cell->pos = Vec2i(r, c);
				}
			}
		}
	}

	void CTableWidget::Construct()
	{
		Super::Construct();
	}

	void CTableWidget::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		GUI::TableFlags flags = GUI::TableFlags_None;
		if (EnumHasFlag(tableFlags, CTableFlags::ResizeableColumns))
			flags |= GUI::TableFlags_Resizable;
		if (EnumHasFlag(tableFlags, CTableFlags::InnerBordersH))
			flags |= GUI::TableFlags_BordersInnerH;
		if (EnumHasFlag(tableFlags, CTableFlags::InnerBordersV))
			flags |= GUI::TableFlags_BordersInnerV;

		if (GUI::BeginTable(rect, GetUuid(), GetName().GetString(), numColumns, flags))
		{
			GUI::PushChildCoordinateSpace(rect);

			for (int r = 0; r < numRows; r++)
			{
				GUI::TableNextRow();
				
				for (int c = 0; c < numColumns; c++)
				{
					GUI::TableNextColumn();

					CTableCellWidget* cell = nullptr;
					if (cellWidgets.KeyExists(Vec2i(r, c)))
					{
						cell = cellWidgets[Vec2i(r, c)];
					}
					else
					{
						GUI::Text("");
						continue;
					}

					auto cursorPos = GUI::GetCursorPos();
					GUI::PushChildCoordinateSpace(cursorPos);
					cell->Render();
					GUI::PopChildCoordinateSpace();
				}
			}

			GUI::PopChildCoordinateSpace();
			GUI::EndTable();
		}
	}

} // namespace CE::Widgets
