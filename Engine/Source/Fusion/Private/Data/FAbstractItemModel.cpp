#include "Fusion.h"

namespace CE
{

    FAbstractItemModel::FAbstractItemModel()
    {

    }

    FAbstractItemModel::~FAbstractItemModel()
    {
        
    }

    FModelIndex FAbstractItemModel::CreateIndex(u32 row, u32 col, void* data)
    {
        FModelIndex index;
        index.row = row;
        index.col = col;
        index.data = data;
        index.model = this;

        return index;
    }

} // namespace CE

