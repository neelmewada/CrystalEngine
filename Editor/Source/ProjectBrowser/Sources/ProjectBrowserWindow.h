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

	FIELD()
	Array<String> recentProjects{};
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

	FUNCTION()
	void OnOpenProjectClicked();

	void OpenProject(const IO::Path& projectPath);

	CTextInput* folderPathInput = nullptr;
	CTextInput* projectNameInput = nullptr;

	CTextInput* openProjectPathInput = nullptr;

	CLabel* errorLabel = nullptr;
	CLabel* openErrorLabel = nullptr;

	FIELD()
	ProjectBrowserCache cache{};

	//String projectName = "";
};


#include "ProjectBrowserWindow.rtti.h"
