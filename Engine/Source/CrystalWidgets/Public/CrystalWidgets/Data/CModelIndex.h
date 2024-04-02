#pragma once

namespace CE::Widgets
{
	class CDataModel;

	struct CRYSTALWIDGETS_API CModelIndex
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

} // namespace CE::Widgets

namespace CE
{
	template<>
	inline SIZE_T GetHash<Widgets::CModelIndex>(const Widgets::CModelIndex& value)
	{
		return GetCombinedHashes({ (SIZE_T)value.GetRow(), (SIZE_T)value.GetColumn(), (SIZE_T)value.GetInternalData() });
	}
}
