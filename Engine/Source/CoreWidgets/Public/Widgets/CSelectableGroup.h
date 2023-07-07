#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CSelectableGroup : public CWidget
	{
		CE_CLASS(CSelectableGroup, CWidget)
	public:

		CSelectableGroup();
		virtual ~CSelectableGroup();

		bool IsContainer() override { return true; }

	protected:

		void OnDrawGUI() override;

	};

} // namespace CE::Widgets

#include "CSelectableGroup.rtti.h"
