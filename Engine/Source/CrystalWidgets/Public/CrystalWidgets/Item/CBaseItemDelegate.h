#pragma once

namespace CE::Widgets
{
    ENUM(Flags)
    enum class CViewItemFeature
    {
	    None = 0,
        HasDecoration = BIT(0),
        HasDisplay = BIT(1),
        AlternateStyle = BIT(2),
    };
    ENUM_CLASS_FLAGS(CViewItemFeature);

    struct CRYSTALWIDGETS_API CViewItemStyle
    {
        CFont font{};
        CViewItemFeature features{};
    };

    CLASS(Abstract)
    class CRYSTALWIDGETS_API CBaseItemDelegate : public Object
    {
        CE_CLASS(CBaseItemDelegate, Object)
    public:

        CBaseItemDelegate();
        virtual ~CBaseItemDelegate();

        virtual void Paint(CPainter* painter, const CViewItemStyle& itemStyle, const CModelIndex& index) = 0;

        virtual Vec2 GetSizeHint(const CViewItemStyle& itemStyle, const CModelIndex& index) = 0;

        virtual void PaintHeader(CPainter* painter, const CViewItemStyle& itemStyle, int position, COrientation orientation, CBaseItemModel* model) = 0;

        virtual Vec2 GetHeaderSizeHint(const CViewItemStyle& itemStyle, int position, COrientation orientation, CBaseItemModel* model) = 0;

        virtual SubClass<CWidget> GetEditorClass(const CViewItemStyle& itemStyle, const CModelIndex& index) { return nullptr; }

        virtual void SetupEditor(CWidget* editor, const CViewItemStyle& itemStyle) {}

        virtual void SetEditorData(CWidget* editor, const CModelIndex& index) {}

    protected:

    };
    
} // namespace CE::Widgets

#include "CBaseItemDelegate.rtti.h"