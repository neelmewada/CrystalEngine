#pragma once

namespace CE::Widgets
{
	class CDataModel;
	class CWidget;
	class CAbstractItemCell;

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

		bool operator==(const CModelIndex& other) const;
		bool operator!=(const CModelIndex& other) const;

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

		virtual String GetHeaderTitle(u32 col) { return ""; }

		virtual u32 GetRowCount(const CModelIndex& parent) = 0;
		virtual u32 GetColumnCount(const CModelIndex& parent) = 0;

		virtual CModelIndex GetParent(const CModelIndex& index) = 0;

		virtual CModelIndex GetIndex(u32 row, u32 col, const CModelIndex& parent = {}) = 0;

		/// The widget class to use for the cell/item
		virtual ClassType* GetWidgetClass(const CModelIndex& index) = 0;

		/// Override this function to set data for an item widget
		virtual void SetData(const CModelIndex& index, CWidget* itemWidget) = 0;

		virtual String GetText(const CModelIndex& index) { return ""; }
        
        virtual int GetIndentLevel(const CModelIndex& index) { return 0; }

		virtual f32 GetCellHeight(const CModelIndex& index) { return 20; }

		virtual void OnIndexSelected(const CModelIndex& index) {}

		CModelIndex CreateIndex(u32 row, u32 col, void* data = nullptr);

		/// Finds the index with matching internal data pointer
		CModelIndex FindIndex(void* internalData, const CModelIndex& parent = {});

		bool IsIndexInParentChain(const CModelIndex& indexToSearch, const CModelIndex& bottomMostIndex);
	};
    
} // namespace CE::Widgets

namespace CE
{
	template<>
	inline SIZE_T GetHash<Widgets::CModelIndex>(const Widgets::CModelIndex& value)
	{
		return GetCombinedHashes({ (SIZE_T)value.GetRow(), (SIZE_T)value.GetColumn(), (SIZE_T)value.GetInternalData() });
	}
}

#include "CDataModel.rtti.h"
