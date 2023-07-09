#pragma once

using namespace CE::Widgets;

CLASS()
class ProjectBrowserWindow : public CWindow
{
	CE_CLASS(ProjectBrowserWindow, CWindow)
public:

	virtual void Construct() override;

private:

	CTextInput* folderPathInput = nullptr;
	CTextInput* projectNameInput = nullptr;

	String projectName = "";
};


#include "ProjectBrowserWindow.rtti.h"
