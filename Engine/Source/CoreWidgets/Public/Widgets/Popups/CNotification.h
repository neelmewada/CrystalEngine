#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CNotification : public CWidget
	{
		CE_CLASS(CNotification, CWidget)
	public:

		CNotification();
		virtual ~CNotification();

		bool IsContainer() override final { return true; }

	protected:

		void OnDrawGUI() override;

	};

} // namespace CE::Widgets

#include "CNotification.rtti.h"
