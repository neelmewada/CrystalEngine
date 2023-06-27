#include "CoreWidgets.h"

namespace CE::Widgets
{

	CTabView::CTabView()
	{
		tabBarId = String::Format("{}", GetUuid());
	}

	CTabView::~CTabView()
	{

	}

	void CTabView::SetTabTitle(CWidget* contentWidget, const String& title)
	{
		if (contentWidget != nullptr)
			SetTabTitle(contentWidget->GetUuid(), title);
	}

	void CTabView::SetTabTitle(UUID contentWidgetUuid, const String& title)
	{
		widgetUuidToTitleMap[contentWidgetUuid] = title;
	}

	void CTabView::OnDrawGUI()
	{
		style.Push();
		bool result = GUI::BeginTabBar(tabBarId, GUI::TabBarFlags_FittingPolicyScroll);
		style.Pop();

		if (result)
		{
			for (int i = 0; i < attachedWidgets.GetSize(); i++)
			{
				UUID uuid = attachedWidgets[i]->GetUuid();
				String title = attachedWidgets[i]->GetName().GetString();

				if (widgetUuidToTitleMap.KeyExists(uuid))
					title = widgetUuidToTitleMap[uuid];

				style.Push();
				bool value = GUI::BeginTabItem(title);
				style.Pop();

				if (value)
				{
					attachedWidgets[i]->RenderGUI();

					GUI::EndTabItem();
				}
			}

			GUI::EndTabBar();
		}
	}

	void CTabView::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
