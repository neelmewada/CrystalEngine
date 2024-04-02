#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CDataModel::CDataModel()
    {
	    
    }

    CDataModel::~CDataModel()
    {
	    
    }

    SubClass<CWidget> CDataModel::GetHeaderWidgetClass(u32 col)
    {
        return GetStaticClass<CLabel>();
    }

    SubClass<CWidget> CDataModel::GetCellWidgetClass(u32 row, u32 col)
    {
        return GetStaticClass<CLabel>();
    }

    CModelIndex CDataModel::CreateIndex(u32 row, u32 col, void* data)
    {
        return CModelIndex(row, col, this, data);
    }
    
} // namespace CE::Widgets
