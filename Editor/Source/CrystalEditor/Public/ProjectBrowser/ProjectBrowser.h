#pragma once

namespace CE::Editor
{

    CLASS(Prefs = Editor)
    class CRYSTALEDITOR_API ProjectBrowser : public CToolWindow
    {
        CE_CLASS(ProjectBrowser, CToolWindow)
    public:

        ProjectBrowser();

        virtual ~ProjectBrowser();

    protected:

        void Construct() override;

        FUNCTION()
        void ValidateInputFields(CTextInput* inputField);

        FUNCTION()
        void OnProjectTemplateSelectionChanged();

        FUNCTION()
        void CreateProject();

        Array<IO::Path> recentProjectPaths{};

        CWidget* newErrorBox = nullptr;
        CLabel* newErrorLabel = nullptr;

        CTabWidget* tabWidget = nullptr;

        CButton* createButton = nullptr;

        CTextInput* newProjectLocation = nullptr;
        CTextInput* newProjectName = nullptr;

        bool isValidInput = true;

        FIELD(Prefs)
        String defaultNewProjectLocation = "";

        FIELD()
        String projectName{};

    };
    
} // namespace CE::Editor

#include "ProjectBrowser.rtti.h"