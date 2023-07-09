#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CLayoutGroup : public CWidget
	{
		CE_CLASS(CLayoutGroup, CWidget)
	public:
		CLayoutGroup();
		virtual ~CLayoutGroup();

		bool IsContainer() override { return true; }

	protected:

		void OnDrawGUI() override;

	};
    
} // namespace CE::Widgets

#include "CLayoutGroup.rtti.h"
