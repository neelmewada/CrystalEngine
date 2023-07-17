#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CContainerWidget : public CWidget
	{
		CE_CLASS(CContainerWidget, CWidget)
	public:

		CContainerWidget();
		virtual ~CContainerWidget();

		bool IsContainer() override { return true; }

		inline bool IsHorizontalScrollAllowed() const { return allowHorizontalScroll; }
		inline void SetHorizontalScrollAllowed(bool set) { allowHorizontalScroll = set; }

		inline bool IsVerticalScrollAllowed() const { return allowVerticalScroll; }
		inline void SetVerticalScrollAllowed(bool value) { allowVerticalScroll = value; }

	protected:

		void OnDrawGUI() override;

		FIELD()
		b8 allowHorizontalScroll = false;

		FIELD()
		b8 allowVerticalScroll = false;
	};

} // namespace CE::Widgets

#include "CContainerWidget.rtti.h"
