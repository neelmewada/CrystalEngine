#pragma once

namespace CE::Widgets
{


	CLASS()
	class COREWIDGETS_API CTabView : public CWidget
	{
		CE_CLASS(CTabView, CWidget)
	public:

		CTabView();
		virtual ~CTabView();

		void SetTabTitle(CWidget* contentWidget, const String& title);
		void SetTabTitle(UUID contentWidgetUuid, const String& title);

	protected:

		virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		String tabBarId{};

		HashMap<UUID, String> widgetUuidToTitleMap{};
	};
    
} // namespace CE::Widgets


#include "CTabView.rtti.h"
