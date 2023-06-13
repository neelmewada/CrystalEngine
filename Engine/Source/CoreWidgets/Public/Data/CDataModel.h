#pragma once

namespace CE::Widgets
{
	class CDataModel;
	class CWidget;

	struct COREWIDGETS_API CModelIndex
	{
	private:
		CModelIndex(u32 row, u32 col, CDataModel* model, void* data = nullptr);

	public:

		CModelIndex() {}

		bool IsValid() const;

		CModelIndex GetParent() const;

		int GetRow() const { return row; }
		int GetColumn() const { return col; }
		void* GetInternalData() const { return data; }

	private:

		friend class CDataModel;

		u32 row = 0;
		u32 col = 0;
		void* data = nullptr;

		CDataModel* model = nullptr;

	};

	typedef Array<CModelIndex> CModelIndexList;

	CLASS(Abstract)
	class COREWIDGETS_API CDataModel : public Object
	{
		CE_CLASS(CDataModel, Object)
	public:

		virtual bool HasHeader() { return false; }

		virtual String GetHeaderTitle(u32 row) { return ""; }

		virtual u32 GetRowCount(const CModelIndex& parent) = 0;
		virtual u32 GetColumnCount(const CModelIndex& parent) = 0;

		virtual CModelIndex GetParent(const CModelIndex& index) = 0;

		virtual CModelIndex GetIndex(u32 row, u32 col, const CModelIndex& parent = {}) = 0;

		/// The widget class to use for the cell/item
		virtual ClassType* GetWidgetClass(const CModelIndex& index) = 0;

		/// Override this function to set data for an item widget
		virtual void SetData(const CModelIndex& index, CWidget* widget) = 0;

		CModelIndex CreateIndex(u32 row, u32 col, void* data = nullptr);

	};
    
} // namespace CE::Widgets


#include "CDataModel.rtti.h"
