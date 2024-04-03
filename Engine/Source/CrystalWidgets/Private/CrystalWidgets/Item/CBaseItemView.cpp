#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CBaseItemView::CBaseItemView()
	{
		defaultSelectionModel = CreateDefaultSubobject<CItemSelectionModel>("ItemSelectionModel");

		if (!IsDefaultInstance())
		{
			selectionModel = defaultSelectionModel;
		}
	}

	CBaseItemView::~CBaseItemView()
	{
		
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


} // namespace CE::Widgets
