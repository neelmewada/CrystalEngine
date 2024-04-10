#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CDockWindow::CDockWindow()
	{
		allowVerticalScroll = true;
		allowHorizontalScroll = false;

		receiveMouseEvents = true;
	}

	CDockWindow::~CDockWindow()
	{
		
	}

} // namespace CE::Widgets
