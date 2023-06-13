#include "CoreWidgets.h"

namespace CE::Widgets
{

	CModelIndex::CModelIndex(u32 row, u32 col, CDataModel* model, void* data)
		: row(row), col(col), model(model), data(data)
	{
	}

	bool CModelIndex::IsValid() const
	{
		return model != nullptr;
	}

	CModelIndex CModelIndex::GetParent() const
	{
		if (model == nullptr)
			return {};
		return model->GetParent(*this);
	}

	CModelIndex CDataModel::CreateIndex(u32 row, u32 col, void* data)
	{
		return CModelIndex(row, col, this, data);
	}

} // namespace CE::Widgets
