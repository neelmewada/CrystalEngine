#pragma once

namespace CE::Widgets
{
    CLASS()
	class CSelectableGroup : public CWidget
	{
		CE_CLASS(CSelectableGroup, CWidget)
	public:

		CSelectableGroup();
		virtual ~CSelectableGroup();

	protected:

		void OnDrawGUI() override;

	};

} // namespace CE::Widgets

#include "CSelectableGroup.rtti.h"
