#pragma once

namespace CE::Widgets
{
	ENUM(Flags)
	enum class CTableFlags
	{
		None = 0,
		ResizeableColumns = BIT(0),
		ReorderableColumns = BIT(1),
		RowBackground = BIT(2),
		ScrollX = BIT(3),
		ScrollY = BIT(4),
		BordersInnerH = BIT(5),   // Draw horizontal borders between rows.
		BordersOuterH = BIT(6),   // Draw horizontal borders at the top and bottom.
		BordersInnerV = BIT(7),   // Draw vertical borders between columns.
		BordersOuterV = BIT(8),  // Draw vertical borders on the left and right sides.
	};
	ENUM_CLASS_FLAGS(CTableFlags);

	CLASS()
	class COREWIDGETS_API CTableView : public CWidget
	{
		CE_CLASS(CTableView, CWidget)
	public:

		inline CDataModel* GetModel() const { return model; }

		inline void SetModel(CDataModel* model) { this->model = model; }

		inline CTableFlags GetTableFlags() const { return tableFlags; }

		inline void SetTableFlags(CTableFlags flags) { tableFlags = flags; }

		inline bool HasAnyTableFlags(CTableFlags flags) const { return (tableFlags & flags) != 0; }
		inline bool HasAllTableFlags(CTableFlags flags) const { return (tableFlags & flags) == flags; }

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		void DrawTableContents(const CModelIndex& parent);

		FIELD()
		CTableFlags tableFlags{};

		FIELD()
		CDataModel* model = nullptr;

		FIELD()
		String id = "";

		FIELD()
		Vec2 tableSize{};

		HashMap<TypeId, Array<CWidget*>> freeWidgetMap{};

		HashMap<CModelIndex, CWidget*> widgetMap{};
	};
    
} // namespace CE::Widgets

#include "CTableView.rtti.h"
