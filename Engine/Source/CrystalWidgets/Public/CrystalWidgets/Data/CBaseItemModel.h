#pragma once

namespace CE::Widgets
{

	enum class CItemDataUsage : u32
	{
		Display = 0,
		Decoration,
	};
	ENUM_CLASS(CItemDataUsage)

	CLASS(Abstract)
	class CRYSTALWIDGETS_API CBaseItemModel : public Object
	{
		CE_CLASS(CBaseItemModel, Object)
	public:

		CBaseItemModel();
		virtual ~CBaseItemModel();

		virtual u32 GetRowCount(const CModelIndex& parent) = 0;
		virtual u32 GetColumnCount(const CModelIndex& parent) = 0;

		virtual CModelIndex GetParent(const CModelIndex& index) = 0;

		virtual CModelIndex GetIndex(u32 row, u32 column, const CModelIndex& parent = {}) = 0;

		virtual Variant GetData(const CModelIndex& index, CItemDataUsage usage = CItemDataUsage::Display) = 0;

		virtual void SetData(const CModelIndex& index, const Variant& data) {}

		CModelIndex CreateIndex(u32 row, u32 col, void* data = nullptr);

		virtual Variant GetHeaderData(int position, COrientation orientation, CItemDataUsage usage = CItemDataUsage::Display) = 0;

		FIELD()
		CVoidEvent onModelDataUpdated{};

	private:


	};

} // namespace CE::Widgets

#include "CBaseItemModel.rtti.h"