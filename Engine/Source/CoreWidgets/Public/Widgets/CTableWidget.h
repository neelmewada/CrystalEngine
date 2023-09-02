#pragma once

namespace CE::Widgets
{

	ENUM(Flags)
	enum class CTableFlags
	{
		None = 0,
		ResizeableColumns = BIT(0),
		ReorderableColumns = BIT(1),
		ScrollX = BIT(3),
		ScrollY = BIT(4),
		InnerBordersH = BIT(5),
		InnerBordersV = BIT(6),
		InnerBorders = InnerBordersH | InnerBordersV,
	};
	ENUM_CLASS_FLAGS(CTableFlags);

	CLASS()
	class COREWIDGETS_API CTableCellWidget : public CWidget
	{
		CE_CLASS(CTableCellWidget, CWidget)
	public:

		CTableCellWidget();
		virtual ~CTableCellWidget();

		bool IsContainer() override { return true; }

		bool IsLayoutCalculationRoot() override { return true; }

		FORCE_INLINE Vec2i GetPosition() const { return pos; }

	protected:

		void OnDrawGUI() override;

		Vec2i pos{};

		friend class CTableWidget;
		friend class CTableView;
	};

	CLASS()
	class COREWIDGETS_API CTableWidget : public CWidget
	{
		CE_CLASS(CTableWidget, CWidget)
	public:

		CTableWidget();
		virtual ~CTableWidget();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		FORCE_INLINE u32 GetNumColumns() const { return numColumns; }
		FORCE_INLINE void SetNumColumns(u32 value) { numColumns = value; CreateOrDestroyCellWidgets(); }

		FORCE_INLINE u32 GetNumRows() const { return numRows; }
		FORCE_INLINE void SetNumRows(u32 value) { numRows = value; CreateOrDestroyCellWidgets(); }

		FORCE_INLINE CTableFlags GetTableFlags() const { return tableFlags; }
		FORCE_INLINE void SetTableFlags(CTableFlags flags) { this->tableFlags = flags; }

		FORCE_INLINE CTableCellWidget* GetCellWidget(Vec2i cell) 
		{ 
			if (!cellWidgets.KeyExists(cell)) 
				return nullptr;
			return cellWidgets[cell]; 
		}

	corewidgets_protected_internal:

		void CreateOrDestroyCellWidgets();

		void Construct() override;

		void OnDrawGUI() override;

		FIELD()
		CTableFlags tableFlags{};

		FIELD()
		u32 numColumns = 1;

		FIELD()
		u32 numRows = 0;

		HashMap<Vec2i, CTableCellWidget*> cellWidgets{};

		Array<float> columnWidths{};
	};
    
} // namespace CE::Widgets

#include "CTableWidget.rtti.h"
