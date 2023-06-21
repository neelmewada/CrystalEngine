#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CGridLayout : public CWidget
	{
		CE_CLASS(CGridLayout, CWidget)
	public:

		CGridLayout();
		virtual ~CGridLayout();


	protected:

		virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	};

} // namespace CE::Widgets


#include "CGridLayout.rtti.h"
