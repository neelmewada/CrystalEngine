#pragma once

namespace CE::Widgets
{

	CLASS()
	class COREWIDGETS_API CTabContainerWidget : public CWidget
	{
		CE_CLASS(CTabContainerWidget, CWidget)
	public:

		CTabContainerWidget();

		inline const String& GetTabTitle() const { return title; }

		void SetTabTitle(const String& title);

		bool IsContainer() override { return true; }

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void OnAfterComputeStyle() override;

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		friend class CTabView;

		FIELD()
		String title = "Tab";
	};

	CLASS()
	class COREWIDGETS_API CTabWidget : public CWidget
	{
		CE_CLASS(CTabWidget, CWidget)
	public:

		CTabWidget();
		virtual ~CTabWidget();

		inline u32 GetItemCount() const { return items.GetSize(); }
		inline CTabContainerWidget* GetItemAt(u32 index) { return items[index]; }

		CTabContainerWidget* GetOrAddTab(const String& tabTitle);

		void OnAfterComputeStyle() override;

	protected:

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		void OnSubWidgetAttached(CWidget* widget) override;
		void OnSubWidgetDetached(CWidget* widget) override;

	private:

		GUI::ID localId = 0;

		Vec4 tabItemPadding = { 10, 5, 10, 5 };

		int curTabIndex = -1;

		FIELD()
		Array<CTabContainerWidget*> items{};
	};
    
} // namespace CE::Widgets

#include "CTabWidget.rtti.h"
