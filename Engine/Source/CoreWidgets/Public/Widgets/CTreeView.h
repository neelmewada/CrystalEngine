#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CTreeItemView : public CWidget
	{
		CE_CLASS(CTreeItemView, CWidget)
	public:

		CTreeItemView();

		virtual ~CTreeItemView();

		bool IsLayoutCalculationRoot() override { return true; }

		void UpdateLayoutIfNeeded() override;

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		inline const String& GetText() const { return label; }
		inline void SetText(const String& text) { this->label = text; }

		void SetIcon(const String& searchPath);

	protected:

		bool ShouldHandleBackgroundDraw() override;

		virtual void OnDrawGUI() override;

		GUI::ID nodeId = 0;

		FIELD()
		b8 isOpen = false;

		FIELD()
		int indent = 0;

		FIELD()
		String label = "";

		CTexture icon{};

		friend class CTreeView;
	};

    CLASS()
    class COREWIDGETS_API CTreeView : public CWidget
    {
        CE_CLASS(CTreeView, CWidget)
    public:

        CTreeView();
        virtual ~CTreeView();

		bool IsContainer() override { return true; }

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		// Getters & Setters

		inline CDataModel* GetModel() const { return model; }
		inline void SetModel(CDataModel* model) { this->model = model; }

		inline bool AlwaysShowVerticalScroll() const { return alwaysShowVerticalScroll; }
		inline void SetAlwaysShowVerticalScroll(bool set) { alwaysShowVerticalScroll = set; }

		inline bool AreItemsSelectable() const { return selectableItems; }
		inline void SetItemsSelectable(bool set) { selectableItems = set; }

		inline CModelIndex GetSelectedIndex() const { return selectedIndex; }
		void Select(const CModelIndex& index, bool expand = true);

    protected:

        virtual void OnDrawGUI() override;

		void DrawChildren(const CModelIndex& parent, int indent = 0);

		FIELD()
		CDataModel* model = nullptr;

		FIELD()
		b8 alwaysShowVerticalScroll = false;

		FIELD()
		b8 selectableItems = false;

		GUI::GuiStyleState itemHovered{};
		GUI::GuiStyleState itemActive{};

		CModelIndex selectedIndex = {};
		CModelIndex indexToExpand = {};

		b8 itemColorFetched = false;

		f32 calculatedHeight = 10;

		HashMap<TypeId, Array<CTreeItemView*>> freeWidgetMap{};
		HashMap<CModelIndex, CTreeItemView*> usedWidgetMap{};
    };
    
} // namespace CE

#include "CTreeView.rtti.h"
