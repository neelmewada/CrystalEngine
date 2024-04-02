#pragma once

namespace CE::Widgets
{

	CLASS(Abstract)
	class CRYSTALWIDGETS_API CDataModel : public Object
	{
		CE_CLASS(CDataModel, Object)
	public:

		CDataModel();
		virtual ~CDataModel();

		virtual bool HasHeader() { return false; }

		virtual String GetHeaderTitle(u32 col) { return ""; }

		virtual SubClass<CWidget> GetHeaderWidgetClass(u32 col);

		virtual SubClass<CWidget> GetCellWidgetClass(u32 row, u32 col);

		virtual u32 GetRowCount(const CModelIndex& parent) = 0;
		virtual u32 GetColumnCount(const CModelIndex& parent) = 0;

		virtual CModelIndex GetParent(const CModelIndex& index) = 0;

		virtual CModelIndex GetIndex(u32 row, u32 col, const CModelIndex& parent = {}) = 0;

		CModelIndex CreateIndex(u32 row, u32 col, void* data = nullptr);

	private:


	};

} // namespace CE::Widgets

#include "CDataModel.rtti.h"