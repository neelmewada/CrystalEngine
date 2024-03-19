#pragma once

namespace CE::Widgets
{
	CLASS()
	class CRYSTALWIDGETS_API CDockWindow : public CWindow
	{
		CE_CLASS(CDockWindow, CWindow)
	public:

		CDockWindow();
		virtual ~CDockWindow();

		CDockType GetDockType() const { return dockType; }

	private:

		FIELD()
		CDockType dockType = CDockType::Major;

	};

} // namespace CE::Widgets

#include "CDockWindow.rtti.h"