#pragma once

namespace CE::Widgets
{
	ENUM(Flags)
	enum class CTableFlags
	{
		None = 0,
		InnerBordersH = BIT(1),
		InnerBordersV = BIT(2),
		InnerBorders = InnerBordersH | InnerBordersV,
		Resizable = BIT(3),
	};
	ENUM_CLASS_FLAGS(CTableFlags)

	CLASS()
	class COREWIDGETS_API CTableCellWidget : public CWidget
	{
		CE_CLASS(CTableCellWidget, CWidget)
	public:

		CTableCellWidget();
		virtual ~CTableCellWidget();

		bool IsContainer() override { return true; }

	protected:

		void OnDrawGUI() override;

	};

	CLASS()
	class COREWIDGETS_API CTableWidget : public CWidget
	{
		CE_CLASS(CTableWidget, CWidget)
	public:

		CTableWidget();
		virtual ~CTableWidget();

		FORCE_INLINE u32 GetNumColumns() const { return numColumns; }
		FORCE_INLINE void SetNumColumns(u32 value) { numColumns = value; }

		FORCE_INLINE u32 GetNumRows() const { return numRows; }
		FORCE_INLINE void SetNumRows(u32 value) { numRows = value; }

		FORCE_INLINE CTableFlags GetTableFlags() const { return tableFlags; }
		FORCE_INLINE void SetTableFlags(CTableFlags flags) { this->tableFlags = flags; }

	protected:

		void Construct() override;

		void OnDrawGUI() override;

		FIELD()
		CTableFlags tableFlags{};

		FIELD()
		u32 numColumns = 1;

		FIELD()
		u32 numRows = 0;

		HashMap<Vec2i, CTableCellWidget*> cellWidgets{};
	};
    
} // namespace CE::Widgets

#include "CTableWidget.rtti.h"
