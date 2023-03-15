
#include "GUI/ViewportWindow.h"

namespace CE::Editor
{

	ViewportWindow::ViewportWindow(QWindow* parent)
		: QWindow(parent)
	{
		setSurfaceType(QSurface::VulkanSurface);
	}

	ViewportWindow::~ViewportWindow()
	{
		
	}

	void ViewportWindow::showEvent(QShowEvent* event)
	{
		
	}

	void ViewportWindow::hideEvent(QHideEvent* event)
	{

	}

} // namespace CE::Editor
