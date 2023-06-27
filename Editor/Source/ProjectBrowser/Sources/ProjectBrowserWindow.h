#pragma once

using namespace CE::Widgets;

CLASS()
class ProjectBrowserWindow : public CWindow
{
	CE_CLASS(ProjectBrowserWindow, CWindow)
public:

	virtual void Construct() override;

private:

	CStackLayout* createContent = nullptr;
	CStackLayout* openContent = nullptr;

	CTextInput* folderPathInput = nullptr;
	CTextInput* projectNameInput = nullptr;
};


#include "ProjectBrowserWindow.rtti.h"