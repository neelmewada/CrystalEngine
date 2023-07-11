#pragma once

using namespace CE::Widgets;

STRUCT()
struct ProjectBrowserCache
{
	CE_STRUCT(ProjectBrowserCache)
public:

	FIELD()
	String lastProjectFolder{};

	FIELD()
	String lastProjectName{};
};

CLASS()
class ProjectBrowserWindow : public CWindow
{
	CE_CLASS(ProjectBrowserWindow, CWindow)
public:

	virtual void Construct() override;

	void SaveCache();

private:

	FUNCTION()
	void OnCreateProjectClicked();

	CTextInput* folderPathInput = nullptr;
	CTextInput* projectNameInput = nullptr;

	CLabel* errorLabel = nullptr;

	FIELD()
	ProjectBrowserCache cache{};

	//String projectName = "";
};


#include "ProjectBrowserWindow.rtti.h"
