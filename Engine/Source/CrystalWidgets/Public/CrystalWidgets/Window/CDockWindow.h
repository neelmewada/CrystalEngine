#pragma once

namespace CE::Widgets
{
	class CDockSpace;
	class CDockSplitView;

	CLASS()
	class CRYSTALWIDGETS_API CDockWindow : public CWindow
	{
		CE_CLASS(CDockWindow, CWindow)
	public:

		CDockWindow();
		virtual ~CDockWindow();

		CDockType GetDockType() const { return dockType; }

		CDockSpace* GetDockSpace() const { return dockSpace; }

	crystalwidgets_internal:

		FIELD()
		CDockType dockType = CDockType::Major;

		FIELD()
		CDockSpace* dockSpace = nullptr;


	};

} // namespace CE::Widgets

#include "CDockWindow.rtti.h"