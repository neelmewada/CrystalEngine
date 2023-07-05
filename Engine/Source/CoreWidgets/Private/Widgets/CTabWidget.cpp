#include "CoreWidgets.h"

namespace CE::Widgets
{
	CTabContainerWidget::CTabContainerWidget()
	{

	}

	void CTabContainerWidget::SetTabTitle(const String& title)
	{
		this->title = title;
	}

	void CTabContainerWidget::OnDrawGUI()
	{

		for (CWidget* subWidget : attachedWidgets)
		{
			subWidget->RenderGUI();
		}

	}

	void CTabContainerWidget::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}




	CTabWidget::CTabWidget()
	{

	}

	CTabWidget::~CTabWidget()
	{

	}

	CTabContainerWidget* CTabWidget::GetOrAddTab(const String& tabTitle)
	{
		if (tabTitle.IsEmpty())
			return nullptr;

		for (auto item : items)
		{
			if (item->GetTabTitle() == tabTitle)
				return item;
		}

		auto tabContainer = CreateWidget<CTabContainerWidget>(this, "TabContainer");
		tabContainer->SetTabTitle(tabTitle);
		return tabContainer;
	}

	void CTabWidget::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		const auto selectedStyle = stylesheet->SelectStyle(this, stateFlags, CSubControl::Tab);
		
		if (selectedStyle.properties.KeyExists(CStylePropertyType::Padding))
		{
			const auto& value = selectedStyle.properties.Get(CStylePropertyType::Padding);
			if (value.IsSingle())
			{
				tabItemPadding = Vec4(1, 1, 1, 1) * value.single;
			}
			else if (value.IsVector())
			{
				tabItemPadding = value.vector;
			}
		}
	}

	Vec2 CTabWidget::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(YGUndefined, YGUndefined);
	}

	void CTabWidget::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		if (localId == 0)
			localId = GenerateRandomU32();

		GUI::PushStyleVar(GUI::StyleVar_FramePadding, (Vec2)tabItemPadding);
		
		bool ret = GUI::BeginTabBar(rect, localId, tabItemPadding);

		GUI::PopStyleVar(1);

		if (ret)
		{
			for (auto item : items)
			{
				if (GUI::BeginTabItem(item->GetTabTitle()))
				{
					item->RenderGUI();
					GUI::EndTabItem();
				}
			}

			GUI::EndTabBar();
		}
	}

	void CTabWidget::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

	void CTabWidget::OnSubWidgetAttached(CWidget* widget)
	{
		Super::OnSubWidgetAttached(widget);

		if (widget->GetClass()->IsSubclassOf<CTabContainerWidget>())
		{
			items.Add((CTabContainerWidget*)widget);
		}
	}

	void CTabWidget::OnSubWidgetDetached(CWidget* widget)
	{
		Super::OnSubWidgetDetached(widget);

		if (widget->GetClass()->IsSubclassOf<CTabContainerWidget>())
		{
			items.Remove((CTabContainerWidget*)widget);
		}
	}

} // namespace CE::Widgets
