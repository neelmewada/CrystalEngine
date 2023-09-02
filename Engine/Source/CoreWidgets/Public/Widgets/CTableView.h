#pragma once

namespace CE::Widgets
{

	CLASS()
	class COREWIDGETS_API CAbstractItemCell : public CWidget
	{
		CE_CLASS(CAbstractItemCell, CWidget)
	public:

		CAbstractItemCell();
		virtual ~CAbstractItemCell();

		inline const String& GetText() const { return text; }
		inline void SetText(const String& text) { this->text = text; }

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

	protected:

		void OnDrawGUI() override;

		FIELD()
		String text{};
	};

    CLASS()
    class COREWIDGETS_API CTableView : public CWidget
    {
        CE_CLASS(CTableView, CWidget)
    public:
        CTableView();
        virtual ~CTableView();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		bool IsContainer() override { return true; }

		// Getters & Setters

		inline CDataModel* GetModel() const { return model; }
		inline void SetModel(CDataModel* model) { this->model = model; }

		inline f32 GetCellHeight() const { return cellHeight; }
		void SetCellHeight(f32 cellHeight);

		inline CTableFlags GetTableFlags() const { return tableFlags; }

		inline void SetTableFlags(CTableFlags flags) { tableFlags = flags; }

		inline bool HasAnyTableFlags(CTableFlags flags) const { return (tableFlags & flags) != 0; }
		inline bool HasAllTableFlags(CTableFlags flags) const { return (tableFlags & flags) == flags; }

	protected:

		void OnDrawGUI() override;

		void DrawTableContents(const CModelIndex& parent);

	private:

		FIELD()
		f32 cellHeight = 10;

		FIELD()
		CDataModel* model = nullptr;

		FIELD()
		CTableFlags tableFlags = CTableFlags::ScrollY;

		HashMap<TypeId, Array<CAbstractItemCell*>> freeWidgetMap{};

		HashMap<CModelIndex, CAbstractItemCell*> widgetMap{};
    };
    
    
} // namespace CE::Widgets

#include "CTableView.rtti.h"
