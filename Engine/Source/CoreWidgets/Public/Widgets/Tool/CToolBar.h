#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CToolBar : public CWidget
	{
		CE_CLASS(CToolBar, CWidget)
	public:

		CToolBar();
		virtual ~CToolBar();

		bool IsContainer() override final { return true; }

	protected:

		void OnDrawGUI() override;

	};

} // namespace CE::Widgets

#include "CToolBar.rtti.h"
