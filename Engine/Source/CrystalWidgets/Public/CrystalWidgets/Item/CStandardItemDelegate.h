#pragma once

namespace CE::Widgets
{
	CLASS()
	class CRYSTALWIDGETS_API CStandardItemDelegate : public CBaseItemDelegate
	{
		CE_CLASS(CStandardItemDelegate, CBaseItemDelegate)
	public:

		CStandardItemDelegate();
		virtual ~CStandardItemDelegate();

		void Paint(CPainter* painter, const CViewItemStyle& itemStyle, const CModelIndex& index) override;

		Vec2 GetSizeHint(const CViewItemStyle& itemStyle, const CModelIndex& index) override;

		void PaintHeader(CPainter* painter, const CViewItemStyle& itemStyle, int position, COrientation orientation, CBaseItemModel* model) override;

		Vec2 GetHeaderSizeHint(const CViewItemStyle& itemStyle, int position, COrientation orientation, CBaseItemModel* model) override;

	protected:

	private:

	};

} // namespace CE::Widgets

#include "CStandardItemDelegate.rtti.h"