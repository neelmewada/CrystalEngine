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

		for (auto widget : attachedWidgets)
		{
			widget->Render();
		}
	}

	CTableWidget::CTableWidget()
	{

	}

	CTableWidget::~CTableWidget()
	{

	}

	void CTableWidget::Construct()
	{
		Super::Construct();
	}

	void CTableWidget::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		GUI::TableFlags flags = GUI::TableFlags_None;
		if (EnumHasFlag(tableFlags, CTableFlags::Resizable))
			flags |= GUI::TableFlags_Resizable;
		if (EnumHasFlag(tableFlags, CTableFlags::InnerBordersH))
			flags |= GUI::TableFlags_BordersInnerH;
		if (EnumHasFlag(tableFlags, CTableFlags::InnerBordersV))
			flags |= GUI::TableFlags_BordersInnerV;

		if (GUI::BeginTable(rect, GetUuid(), GetName().GetString(), numColumns, flags))
		{
			for (int r = 0; r < numRows; r++)
			{
				GUI::TableNextRow();
				
				for (int c = 0; c < numColumns; c++)
				{
					GUI::TableNextColumn();

					CTableCellWidget* cell = nullptr;
					if (!cellWidgets.KeyExists(Vec2i(r, c)))
					{
						cell = CreateWidget<CTableCellWidget>(this, "TableCell");
						cellWidgets[Vec2i(r, c)] = cell;
					}
					else
					{
						cell = cellWidgets[Vec2i(r, c)];
					}

					auto cursorPos = GUI::GetCursorPos();
					GUI::PushChildCoordinateSpace(cursorPos);
					cell->Render();
					GUI::PopChildCoordinateSpace();
				}
			}

			GUI::EndTable();
		}
	}

} // namespace CE::Widgets
