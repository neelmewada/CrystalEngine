#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CScrollView : public CWidget
	{
		CE_CLASS(CScrollView, CWidget)
	public:

		CScrollView();
		virtual ~CScrollView();

		bool IsContainer() override { return true; }

		inline bool AlwaysShowVerticalScroll() const { return alwaysShowVerticalScroll; }
		inline void SetAlwaysShowVerticalScroll(bool set) { alwaysShowVerticalScroll = set; }


	protected:

		void OnDrawGUI() override;

		FIELD()
		b8 alwaysShowVerticalScroll = false;

	};
    
} // namespace CE::Widgets

#include "CScrollView.rtti.h"
