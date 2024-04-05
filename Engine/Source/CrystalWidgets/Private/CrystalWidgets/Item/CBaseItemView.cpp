#include "CrystalWidgets.h"

namespace CE::Widgets
{
	constexpr f32 DefaultDecorationSize = 10.0f;

	constexpr f32 ScrollRectWidth = 10.0f;
	constexpr f32 MinScrollRectSize = 20.0f;
	constexpr f32 ScrollSizeBuffer = 1.0f;

	CBaseItemView::CBaseItemView()
	{
		defaultSelectionModel = CreateDefaultSubobject<CItemSelectionModel>("ItemSelectionModel");
		delegate = CreateDefaultSubobject<CStandardItemDelegate>("StandardItemDelegate");

		if (!IsDefaultInstance())
		{
			selectionModel = defaultSelectionModel;
		}
	}

	CBaseItemView::~CBaseItemView()
	{
		
	}

	Vec2 CBaseItemView::CalculateIntrinsicSize(f32 width, f32 height)
	{
		if (!model || !delegate)
			return Vec2();

		u32 numColumns = model->GetColumnCount({});
		u32 numRows = model->GetRowCount({});

		if (numColumns == 0)
			return Vec2();

		bool hasColumnHeader = true;

		for (int i = 0; i < numColumns; ++i)
		{
			Variant headerData = model->GetHeaderData(i, COrientation::Horizontal);
			if (!headerData.HasValue())
			{
				hasColumnHeader = false;
				break;
			}
		}

		bool hasRowHeader = true;

		for (int i = 0; i < numRows; ++i)
		{
			Variant headerData = model->GetHeaderData(i, COrientation::Vertical);
			if (!headerData.HasValue())
			{
				hasRowHeader = false;
				break;
			}
		}

		if (!canDrawColumnHeader)
			hasColumnHeader = false;
		if (!canDrawRowHeader)
			hasRowHeader = false;

		CStyle headerStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::Header);
		CStyle headerHoveredStyle = styleSheet->SelectStyle(this, CStateFlag::Hovered, CSubControl::Header);

		Vec2 size = Vec2();

		columnWidths.Resize(numColumns);
		columnHeaderHeight = 0;

		for (int col = 0; col < numColumns; ++col)
		{
			columnWidths[col] = 0;

			if (hasColumnHeader)
			{
				CViewItemStyle headerViewStyle{};
				Variant headerText = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Display);
				if (headerText.IsTextType())
				{
					headerViewStyle.text = headerText.GetTextValue();
				}

				if (hoveredColumnHeader == col) // Hovered column
				{
					headerViewStyle.padding = headerHoveredStyle.properties[CStylePropertyType::Padding].vector;
					headerViewStyle.font.SetFamily(headerHoveredStyle.GetFontName());
					headerViewStyle.font.SetSize(headerHoveredStyle.GetFontSize());
					headerViewStyle.features = CViewItemFeature::HasDisplay;

					Variant headerDisplay = model->GetHeaderData(col, COrientation::Horizontal);
					Variant headerIcon = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Decoration);

					if (headerIcon.HasValue() && headerIcon.IsTextType())
					{
						headerViewStyle.features |= CViewItemFeature::HasDecoration;
						headerViewStyle.icon = headerIcon.GetNameValue();
						headerViewStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
					}

					if (headerDisplay.IsTextType())
					{
						headerViewStyle.text = headerDisplay.GetTextValue();
					}
				}
				else
				{
					headerViewStyle.padding = headerStyle.properties[CStylePropertyType::Padding].vector;
					headerViewStyle.font.SetFamily(headerStyle.GetFontName());
					headerViewStyle.font.SetSize(headerStyle.GetFontSize());
					headerViewStyle.features = CViewItemFeature::HasDisplay;

					Variant headerDisplay = model->GetHeaderData(col, COrientation::Horizontal);
					Variant headerIcon = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Decoration);

					if (headerIcon.HasValue() && headerIcon.IsTextType())
					{
						headerViewStyle.features |= CViewItemFeature::HasDecoration;
						headerViewStyle.icon = headerIcon.GetNameValue();
						headerViewStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
					}

					if (headerDisplay.IsTextType())
					{
						headerViewStyle.text = headerDisplay.GetTextValue();
					}
				}

				Vec2 headerSize = delegate->GetHeaderSizeHint(headerViewStyle, col, COrientation::Horizontal, model);

				columnHeaderHeight = Math::Max(columnHeaderHeight, headerSize.height);
			}

			for (int row = 0; row < numRows; ++row)
			{
				
			}
		}
		
		return size;
	}

	void CBaseItemView::SetSelectionModel(CItemSelectionModel* selectionModel)
	{
		this->selectionModel = selectionModel;
	}

	void CBaseItemView::SetModel(CBaseItemModel* model)
	{
		this->model = model;

		if (selectionModel)
			selectionModel->SetModel(model);
	}

	void CBaseItemView::SetDelegate(CBaseItemDelegate* delegate)
	{
		this->delegate = delegate;
	}

	void CBaseItemView::Construct()
	{
		Super::Construct();

	}

	void CBaseItemView::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);

		if (!model)
			return;

		CPainter* painter = paintEvent->painter;

		u32 numColumns = model->GetColumnCount({});
		u32 numRows = model->GetRowCount({});

		if (numColumns == 0)
			return;

		painter->PushChildCoordinateSpace(GetComputedLayoutTopLeft());
		Vec2 regionSize = GetComputedLayoutSize();
		painter->PushClipRect(Rect::FromSize(Vec2(), regionSize));

		bool hasColumnHeader = true;

		for (int i = 0; i < numColumns; ++i)
		{
			Variant headerData = model->GetHeaderData(i, COrientation::Horizontal);
			if (!headerData.HasValue())
			{
				hasColumnHeader = false;
				break;
			}
		}

		bool hasRowHeader = true;

		for (int i = 0; i < numRows; ++i)
		{
			Variant headerData = model->GetHeaderData(i, COrientation::Vertical);
			if (!headerData.HasValue())
			{
				hasRowHeader = false;
				break;
			}
		}

		if (!canDrawColumnHeader)
			hasColumnHeader = false;
		if (!canDrawRowHeader)
			hasRowHeader = false;

		CStyle headerStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::Header);
		CStyle headerHoveredStyle = styleSheet->SelectStyle(this, CStateFlag::Hovered, CSubControl::Header);
		CStyle alternateStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::Alternate);
		alternateBgColor = alternateStyle.properties[CStylePropertyType::Background].color;

		if (rowHeightsByParent[{}].GetSize() != numRows)
			recalculateRows = true;

		columnWidths.Resize(numColumns);
		columnWidthRatios.Reserve(numColumns);
		rowHeightsByParent[{}].Resize(numRows);
		columnHeaderHeight = 0;

		f32 remainingColumnWidth = regionSize.width;

		for (int col = 0; col < numColumns; ++col)
		{
			if (EnumHasFlag(columnHeaderFlags[col], CItemViewHeaderFlags::Resizable))
			{
				columnWidths[col] = 0;
			}
			else
			{
				remainingColumnWidth -= columnWidths[col];
			}

			if (hasColumnHeader)
			{
				CViewItemStyle headerViewStyle{};
				Variant headerText = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Display);
				if (headerText.IsTextType())
				{
					headerViewStyle.text = headerText.GetTextValue();
				}

				if (hoveredColumnHeader == col) // Hovered column
				{
					headerViewStyle.padding = headerHoveredStyle.properties[CStylePropertyType::Padding].vector;
					headerViewStyle.font.SetFamily(headerHoveredStyle.GetFontName());
					headerViewStyle.font.SetSize(headerHoveredStyle.GetFontSize());
					headerViewStyle.features = CViewItemFeature::HasDisplay;

					Variant headerDisplay = model->GetHeaderData(col, COrientation::Horizontal);
					Variant headerIcon = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Decoration);

					if (headerIcon.HasValue() && headerIcon.IsTextType())
					{
						headerViewStyle.features |= CViewItemFeature::HasDecoration;
						headerViewStyle.icon = headerIcon.GetNameValue();
						headerViewStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
					}

					if (headerDisplay.IsTextType())
					{
						headerViewStyle.text = headerDisplay.GetTextValue();
					}
				}
				else
				{
					headerViewStyle.padding = headerStyle.properties[CStylePropertyType::Padding].vector;
					headerViewStyle.font.SetFamily(headerStyle.GetFontName());
					headerViewStyle.font.SetSize(headerStyle.GetFontSize());
					headerViewStyle.features = CViewItemFeature::HasDisplay;

					Variant headerDisplay = model->GetHeaderData(col, COrientation::Horizontal);
					Variant headerIcon = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Decoration);

					if (headerIcon.HasValue() && headerIcon.IsTextType())
					{
						headerViewStyle.features |= CViewItemFeature::HasDecoration;
						headerViewStyle.icon = headerIcon.GetNameValue();
						headerViewStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
					}

					if (headerDisplay.IsTextType())
					{
						headerViewStyle.text = headerDisplay.GetTextValue();
					}
				}

				Vec2 headerSize = delegate->GetHeaderSizeHint(headerViewStyle, col, COrientation::Horizontal, model);

				columnHeaderHeight = Math::Max(columnHeaderHeight, headerSize.height);
			}
		}

		if (recalculateRows)
		{
			cellStyle = styleSheet->SelectStyle(this, CStateFlag::Default, CSubControl::Cell);
			cellHoveredStyle = styleSheet->SelectStyle(this, CStateFlag::Hovered, CSubControl::Cell);
			cellSelectedStyle = styleSheet->SelectStyle(this, CStateFlag::Active, CSubControl::Cell);
			totalContentHeight = 0.0f;
			expandableColumn = -1;

			CalculateRowHeights(rowHeightsByParent[CModelIndex()], CModelIndex());

			contentSize.height = totalContentHeight;
			contentSize.width = regionSize.width;
		}

		for (int col = 0; col < numColumns; ++col)
		{
			if (EnumHasFlag(columnHeaderFlags[col], CItemViewHeaderFlags::Resizable))
			{
				columnWidths[col] = columnWidthRatios[col] * remainingColumnWidth;
			}
		}

		// - Paint rows & cells

		scrollOffset.y = Math::Clamp(scrollOffset.y, 0.0f, totalContentHeight - contentSize.height - columnHeaderHeight);
		if (totalContentHeight <= contentSize.height - columnHeaderHeight)
			scrollOffset.y = 0.0f;

		painter->PushChildCoordinateSpace(Vec2(0, columnHeaderHeight - scrollOffset.y));
		painter->PushClipRect(Rect::FromSize(0, 0, regionSize.width, contentSize.height - columnHeaderHeight));

		PaintRows(painter, Rect::FromSize(0, 0, regionSize.width, regionSize.height - columnHeaderHeight), 0, CModelIndex());

		painter->PopClipRect();
		painter->PopChildCoordinateSpace();

		Vec2 headerOrigin = Vec2();

		// - Paint headers
		for (int col = 0; hasColumnHeader && col < numColumns; ++col)
		{
			CViewItemStyle headerViewStyle{};
			Variant headerText = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Display);
			if (headerText.IsTextType())
			{
				headerViewStyle.text = headerText.GetTextValue();
			}

			if (hoveredColumnHeader == col) // Hovered column
			{
				headerViewStyle.padding = headerHoveredStyle.properties[CStylePropertyType::Padding].vector;
				headerViewStyle.font.SetFamily(headerHoveredStyle.GetFontName());
				headerViewStyle.font.SetSize(headerHoveredStyle.GetFontSize());
				headerViewStyle.features = CViewItemFeature::HasDisplay;
				headerViewStyle.bgColor = headerHoveredStyle.properties[CStylePropertyType::Background].color;
				headerViewStyle.textColor = headerHoveredStyle.properties[CStylePropertyType::Foreground].color;

				Variant headerDisplay = model->GetHeaderData(col, COrientation::Horizontal);
				Variant headerIcon = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Decoration);

				if (headerIcon.HasValue() && headerIcon.IsTextType())
				{
					headerViewStyle.features |= CViewItemFeature::HasDecoration;
					headerViewStyle.icon = headerIcon.GetNameValue();
					headerViewStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
				}

				if (headerDisplay.IsTextType())
				{
					headerViewStyle.text = headerDisplay.GetTextValue();
				}
			}
			else
			{
				headerViewStyle.padding = headerStyle.properties[CStylePropertyType::Padding].vector;
				headerViewStyle.font.SetFamily(headerStyle.GetFontName());
				headerViewStyle.font.SetSize(headerStyle.GetFontSize());
				headerViewStyle.features = CViewItemFeature::HasDisplay;
				headerViewStyle.bgColor = headerStyle.properties[CStylePropertyType::Background].color;
				headerViewStyle.textColor = headerStyle.properties[CStylePropertyType::Foreground].color;

				Variant headerDisplay = model->GetHeaderData(col, COrientation::Horizontal);
				Variant headerIcon = model->GetHeaderData(col, COrientation::Horizontal, CItemDataUsage::Decoration);

				if (headerIcon.HasValue() && headerIcon.IsTextType())
				{
					headerViewStyle.features |= CViewItemFeature::HasDecoration;
					headerViewStyle.icon = headerIcon.GetNameValue();
					headerViewStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
				}

				if (headerDisplay.IsTextType())
				{
					headerViewStyle.text = headerDisplay.GetTextValue();
				}
			}

			Vec2 headerSize = Vec2(columnWidths[col], columnHeaderHeight);
			
			painter->PushChildCoordinateSpace(headerOrigin);
			painter->PushClipRect(Rect::FromSize(Vec2(), headerSize));

			delegate->PaintHeader(painter, headerSize, headerViewStyle, col, COrientation::Horizontal, model);

			painter->PopClipRect();
			painter->PopChildCoordinateSpace();

			headerOrigin += Vec2(headerSize.width, 0);
		}

		painter->PopClipRect();
		painter->PopChildCoordinateSpace();

		recalculateRows = false;
	}

	void CBaseItemView::PaintRows(CPainter* painter, const Rect& regionRect, int indentLevel, const CModelIndex& parentIndex)
	{
		int numRows = model->GetRowCount(parentIndex);
		int numColumns = model->GetColumnCount(parentIndex);

		f32 rowPosY = 0.0f;
		CFont font{};

		font.SetFamily(cellStyle.GetFontName());
		if (!font.GetFamily().IsValid())
		{
			font.SetFamily(computedStyle.GetFontName());
		}

		font.SetSize(cellStyle.GetFontSize());

		for (int row = 0; row < numRows; ++row)
		{
			if (rowPosY > regionRect.GetSize().height)
			{
				return; // Out of bounds
			}

			if (row % 2 != 0 && alternateBgColor.a > 0)
			{
				// TODO: Paint alternate Background
			}

			if (selectionModel != nullptr && selectionType == CItemSelectionType::SelectRow)
			{
				for (int col = 0; col < numColumns; ++col)
				{
					CModelIndex index = model->GetIndex(row, col, parentIndex);
					if (selectionModel->IsSelected(index))
					{
						CBrush brush = CBrush(Color::RGBA(0, 112, 224));
						painter->SetBrush(brush);
						painter->SetPen(CPen());

						painter->DrawRect(Rect::FromSize(0, rowPosY, regionRect.GetSize().width, ceil(rowHeightsByParent[parentIndex][row])));

						break;
					}
				}
			}

			f32 posX = 0.0f;

			for (int col = 0; col < numColumns; ++col)
			{
				CModelIndex index = model->GetIndex(row, col, parentIndex);
				CViewItemStyle itemStyle{};
				itemStyle.font = font;
				
				Variant display = model->GetData(index, CItemDataUsage::Display);
				if (display.HasValue() && display.IsTextType())
				{
					itemStyle.features |= CViewItemFeature::HasDisplay;
					itemStyle.text = display.GetTextValue();
				}

				Variant icon = model->GetData(index, CItemDataUsage::Decoration);
				if (icon.HasValue() && icon.IsTextType())
				{
					itemStyle.features |= CViewItemFeature::HasDecoration;
					itemStyle.icon = icon.GetNameValue();
					itemStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
				}

				itemStyle.padding = cellStyle.GetPadding();
				itemStyle.textColor = computedStyle.GetForegroundColor();
				itemStyle.bgColor = Color::Clear();
				itemStyle.isExpanded = expandedRows.Exists(index);
				itemStyle.expandableColumn = expandableColumn;

				Rect columnRect = Rect::FromSize(posX, rowPosY, columnWidths[col], rowHeightsByParent[parentIndex][row]);

				painter->PushChildCoordinateSpace(columnRect.min);
				painter->PushClipRect(Rect::FromSize(Vec2(0, 0), columnRect.GetSize()));

				delegate->Paint(painter, itemStyle, index);

				painter->PopClipRect();
				painter->PopChildCoordinateSpace();

				if (itemStyle.isExpanded)
				{
					PaintRows(painter, regionRect, indentLevel + 1, index);
				}

				posX += columnWidths[col];
			}

			rowPosY += rowHeightsByParent[parentIndex][row];
		}
	}

	void CBaseItemView::CalculateRowHeights(Array<f32>& outHeights, const CModelIndex& parentIndex)
	{
		outHeights.Clear();

		int numRows = model->GetRowCount(parentIndex);
		int numColumns = model->GetColumnCount(parentIndex);

		if (numColumns == 0 || numRows == 0)
			return;

		outHeights.Reserve(numRows);
		CFont font{};

		font.SetFamily(cellStyle.GetFontName());
		if (!font.GetFamily().IsValid())
		{
			font.SetFamily(computedStyle.GetFontName());
		}

		font.SetSize(cellStyle.GetFontSize());

		for (int row = 0; row < numRows; ++row)
		{
			f32 height = 0;

			for (int col = 0; col < numColumns; ++col)
			{
				CModelIndex index = model->GetIndex(row, col, parentIndex);

				if (model->GetRowCount(index) > 0)
				{
					if (expandableColumn == -1)
					{
						expandableColumn = col;
					}

					if (expandedRows.Exists(index))
					{
						CalculateRowHeights(rowHeightsByParent[index], index);
					}
				}

				CViewItemStyle itemStyle{};
				itemStyle.font = font;

				Variant display = model->GetData(index, CItemDataUsage::Display);
				if (display.HasValue() && display.IsTextType())
				{
					itemStyle.features |= CViewItemFeature::HasDisplay;
					itemStyle.text = display.GetTextValue();
				}

				Variant icon = model->GetData(index, CItemDataUsage::Decoration);
				if (icon.HasValue() && icon.IsTextType())
				{
					itemStyle.features |= CViewItemFeature::HasDecoration;
					itemStyle.icon = icon.GetNameValue();
					itemStyle.decorationRect = Rect::FromSize(0, 0, DefaultDecorationSize, DefaultDecorationSize);
				}

				itemStyle.padding = cellStyle.GetPadding();
				itemStyle.textColor = computedStyle.GetForegroundColor();
				itemStyle.bgColor = Color::Clear();
				itemStyle.expandableColumn = expandableColumn;

				Vec2 size = delegate->GetSizeHint(itemStyle, index);

				height = Math::Max(height, size.height);
			}

			totalContentHeight += height;
			outHeights.Add(height);
		}
	}

	void CBaseItemView::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		Super::OnPaintOverlay(paintEvent);

		if (!model)
			return;
	}

	void CBaseItemView::SetColumFixedWidth(u32 column, f32 width)
	{
		columnWidths.Resize(column + 1, 0);
		columnWidths[column] = width;

		columnHeaderFlags.Resize(column + 1, CItemViewHeaderFlags::None);
		columnHeaderFlags[column] &= ~CItemViewHeaderFlags::Resizable;
	}

	void CBaseItemView::SetColumnResizable(u32 column, f32 widthRatio)
	{
		columnWidthRatios.Resize(column + 1);
		columnWidthRatios[column] = widthRatio;

		columnHeaderFlags.Resize(column + 1, CItemViewHeaderFlags::None);
		columnHeaderFlags[column] |= CItemViewHeaderFlags::Resizable;
	}

	Rect CBaseItemView::GetVerticalScrollRect()
	{
		if (allowVerticalScroll)
		{
			Vec2 originalSize = GetComputedLayoutSize();
			f32 originalHeight = originalSize.height;
			f32 contentMaxY = contentSize.height;

			if (contentMaxY > originalHeight + ScrollSizeBuffer)
			{
				Rect scrollRegion = Rect::FromSize(Vec2(originalSize.width - ScrollRectWidth, 0), Vec2(ScrollRectWidth, originalHeight));
				f32 scrollRectHeightRatio = originalHeight / contentMaxY;

				normalizedScroll.y = Math::Clamp01(scrollOffset.y);

				Rect scrollRect = Rect::FromSize(scrollRegion.min,
					Vec2(scrollRegion.GetSize().width, Math::Max(scrollRegion.GetSize().height * scrollRectHeightRatio, MinScrollRectSize)));
				scrollRect = scrollRect.Translate(Vec2(0, (originalHeight - scrollRect.GetSize().height) * normalizedScroll.y));

				return scrollRect;
			}
		}

		return Rect();
	}

	void CBaseItemView::HandleEvent(CEvent* event)
	{
		if (event->IsMouseEvent())
		{
			CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

			mouseEvent->Consume(this);

			if (mouseEvent->type == CEventType::MouseEnter || mouseEvent->type == CEventType::MouseMove)
			{
				
			}
			else if (mouseEvent->type == CEventType::MouseLeave)
			{
				
			}

			SetNeedsPaint();
		}
		else if (event->IsKeyEvent())
		{
			CKeyEvent* keyEvent = static_cast<CKeyEvent*>(event);

			keyEvent->Consume(this);

			SetNeedsPaint();
		}

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
