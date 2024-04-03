#include "CrystalWidgets.h"

namespace CE::Widgets
{

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
		if (!model)
			return Vec2();

		Vec2 size = Vec2();
		// TODO: Calculate size
		
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

		u32 numColumns = model->GetColumnCount({});
		u32 numRows = model->GetRowCount({});

		if (numColumns == 0 || numRows == 0)
			return;

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


	}

	void CBaseItemView::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		Super::OnPaintOverlay(paintEvent);

		if (!model)
			return;
	}

	void CBaseItemView::HandleEvent(CEvent* event)
	{


		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
