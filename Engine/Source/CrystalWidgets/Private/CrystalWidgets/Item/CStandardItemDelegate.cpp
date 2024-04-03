#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CStandardItemDelegate::CStandardItemDelegate()
    {
	    
    }

    CStandardItemDelegate::~CStandardItemDelegate()
    {
	    
    }

    void CStandardItemDelegate::Paint(CPainter* painter, const CViewItemStyle& itemStyle, const CModelIndex& index)
    {
        CBaseItemModel* model = index.GetModel();
        if (!model)
            return;

        
    }

    Vec2 CStandardItemDelegate::GetSizeHint(const CViewItemStyle& itemStyle, const CModelIndex& index)
    {
        if (!index.IsValid())
            return Vec2();

        Vec2 size = Vec2();

        return size;
    }

    void CStandardItemDelegate::PaintHeader(CPainter* painter, const CViewItemStyle& itemStyle, int position,
	    COrientation orientation, CBaseItemModel* model)
    {

    }

    Vec2 CStandardItemDelegate::GetHeaderSizeHint(const CViewItemStyle& itemStyle, int position,
	    COrientation orientation, CBaseItemModel* model)
    {
        return Vec2();
    }

} // namespace CE::Widgets
