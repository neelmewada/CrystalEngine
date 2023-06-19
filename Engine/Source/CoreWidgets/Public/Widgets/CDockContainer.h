#pragma once

namespace CE::Widgets
{
    
	CLASS()
	class COREWIDGETS_API CDockContainer : public CWindow
	{
		CE_CLASS(CDockContainer, CWindow)
	public:

		CDockContainer();
		virtual ~CDockContainer();

		inline void DisableDock(bool disable)
		{
			disableDock = disable;
		}

		inline bool IsDockDisabled() const
		{
			return disableDock;
		}

	protected:

		virtual void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		FIELD()
		b8 hasMenu = false;

		FIELD()
		b8 disableDock = false;
	};

} // namespace CE::Widgets


#include "CDockContainer.rtti.h"
