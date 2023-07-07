#include "CoreWidgets.h"

namespace CE::Widgets
{
	CTabContainerWidget::CTabContainerWidget()
	{
		SetTabTitle("Tab");
	}

	void CTabContainerWidget::SetTabTitle(const String& title)
	{
		this->title = String::Format("{}##{}", title, GetUuid());
	}

	Vec2 CTabContainerWidget::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(width, height);
	}

	void CTabContainerWidget::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		auto paddingTop = YGNodeStyleGetPadding(node, YGEdgeTop);
		if (paddingTop.unit == YGUnitPoint)
		{
			//YGNodeStyleSetPadding(node, YGEdgeTop, 20 + paddingTop.value);
		}
		else if (paddingTop.unit == YGUnitUndefined)
		{
			//YGNodeStyleSetPadding(node, YGEdgeTop, 20);
		}
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

		auto selectedStyle = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Tab);
		{
			const auto& value = selectedStyle.properties[CStylePropertyType::Background];
			if (value.IsColor())
			{
				tab = value.color;
			}
		}
		
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

		auto paddingTop = YGNodeStyleGetPadding(node, YGEdgeTop);
		if (paddingTop.unit == YGUnitPoint)
		{
			YGNodeStyleSetPadding(node, YGEdgeTop, tabItemPadding.top + 20 + paddingTop.value);
		}
		else if (paddingTop.unit == YGUnitUndefined)
		{
			YGNodeStyleSetPadding(node, YGEdgeTop, tabItemPadding.top + 20);
		}

		auto hoveredTabStyle = stylesheet->SelectStyle(this, CStateFlag::Hovered, CSubControl::Tab);
		{
			const auto& value = hoveredTabStyle.properties[CStylePropertyType::Background];
			if (value.IsColor())
			{
				tabHovered = value.color;
			}
		}

		auto activeTabStyle = stylesheet->SelectStyle(this, CStateFlag::Active, CSubControl::Tab);
		{
			const auto& value = activeTabStyle.properties[CStylePropertyType::Background];
			if (value.IsColor())
			{
				tabActive = value.color;
			}
		}

		auto unfocusedTabStyle = stylesheet->SelectStyle(this, CStateFlag::Unfocused, CSubControl::Tab);
		{
			const auto& value = unfocusedTabStyle.properties[CStylePropertyType::Background];
			if (value.IsColor())
			{
				tabUnfocused = value.color;
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

		int pushColors = 0;
		if (tab.a > 0)
		{
			GUI::PushStyleColor(GUI::StyleCol_Tab, tab);
			pushColors++;
		}
		if (tabHovered.a > 0)
		{
			GUI::PushStyleColor(GUI::StyleCol_TabHovered, tabHovered);
			pushColors++;
		}
		if (tabActive.a > 0)
		{
			GUI::PushStyleColor(GUI::StyleCol_TabActive, tabActive);
			GUI::PushStyleColor(GUI::StyleCol_TabUnfocusedActive, tabActive);
			pushColors += 2;
		}
		if (tabUnfocused.a > 0)
		{
			GUI::PushStyleColor(GUI::StyleCol_TabUnfocused, tabUnfocused);
			pushColors++;
		}
		
		bool ret = GUI::BeginTabBar(rect, localId, tabItemPadding, GUI::TabBarFlags_FittingPolicyScroll);

		GUI::PopStyleVar(1);

		if (ret)
		{
			for (int i = 0; i < items.GetSize(); i++)
			{
				auto item = items[i];

				GUI::PushStyleVar(GUI::StyleVar_FramePadding, (Vec2)tabItemPadding);
				bool value = GUI::BeginTabItem(item->GetTabTitle(), tabItemPadding);
				GUI::PopStyleVar(1);

				if (value)
				{
					if (curTabIndex != i)
					{
						curTabIndex = i;
						item->SetNeedsStyle();
						item->SetNeedsLayout();
					}

					item->RenderGUI();
					GUI::EndTabItem();
				}
			}

			GUI::EndTabBar();
		}

		if (pushColors > 0)
			GUI::PopStyleColor(pushColors);
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
