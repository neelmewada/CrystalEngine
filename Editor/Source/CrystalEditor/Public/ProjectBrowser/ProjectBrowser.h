#pragma once

namespace CE::Editor
{

    CLASS(Prefs = Editor)
    class CRYSTALEDITOR_API ProjectBrowser : public FToolWindow
    {
        CE_CLASS(ProjectBrowser, FToolWindow)
    public:

        ProjectBrowser();

        virtual ~ProjectBrowser();

        void OnPostComputeLayout() override;

    protected:

        void Construct() override;

        FListItemWidget& GenerateRow(FListItem* item, FListView* view);

        FUNCTION()
        void ValidateInputFields(FTextInput* inputField);

        FUNCTION()
        void OnProjectTemplateSelectionChanged();

        FUNCTION()
        void OnRecentProjectSelectionChanged();

        FUNCTION()
        void CreateProject();

        FUNCTION()
        void OpenProject();

        Array<IO::Path> recentProjectPaths{};

        FIELD()
        RecentProjectsModel* recentProjectsModel = nullptr;

        FListView* recentsList = nullptr;
        FListView* newProjectList = nullptr;

        /*CWidget* newErrorBox = nullptr;
        CLabel* newErrorLabel = nullptr;

        CWidget* openErrorBox = nullptr;
        CLabel* openErrorLabel = nullptr;

        CTabWidget* tabWidget = nullptr;
        CListWidget* recentsList = nullptr;

        CButton* createButton = nullptr;
        CButton* openButton = nullptr;

        CTextInput* openProjectLocation = nullptr;

        CTextInput* newProjectLocation = nullptr;
        CTextInput* newProjectName = nullptr;*/

        bool isValidInput = true;

        // - Prefs -
        // Preferences are automatically cached and saved to AppData folder and
        // reloaded whenever a ProjectBrowser object is created.

        FIELD(Prefs)
        String defaultOpenProjectLocation = "";

        FIELD(Prefs)
        String defaultNewProjectLocation = "";

        // - Fields -

        FIELD()
        String projectName{};

    };
    
} // namespace CE::Editor

#include "ProjectBrowser.rtti.h"