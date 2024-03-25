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

		CDockSpace* GetDockSpace() const { return dockSpace; }

		bool IsMainWindow() const { return isMainWindow; }

		void SetAsMainWindow(bool set) { isMainWindow = set; }
		
	crystalwidgets_internal:

		FIELD()
		CDockSpace* dockSpace = nullptr;

		FIELD()
		b8 isMainWindow = false;

	};

} // namespace CE::Widgets

#include "CDockWindow.rtti.h"