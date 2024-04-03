#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CBaseItemModel::CBaseItemModel()
    {
	    
    }

    CBaseItemModel::~CBaseItemModel()
    {
	    
    }

    CModelIndex CBaseItemModel::CreateIndex(u32 row, u32 col, void* data)
    {
        return CModelIndex(row, col, this, data);
    }
    
} // namespace CE::Widgets
