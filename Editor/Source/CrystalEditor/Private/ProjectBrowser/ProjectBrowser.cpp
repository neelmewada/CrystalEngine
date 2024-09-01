#include "CrystalEditor.h"

namespace CE::Editor
{

    ProjectBrowser::ProjectBrowser()
    {
        
    }

    ProjectBrowser::~ProjectBrowser()
    {
	    
    }

    void ProjectBrowser::Construct()
    {
	    Super::Construct();

        
        
        ProjectManager* projectManager = ProjectManager::Get();

        CTabWidgetContainer* recentsTab = CreateObject<CTabWidgetContainer>(tabWidget, "RecentsTab");
        recentsTab->SetTitle("Recent Projects");
	    {
            recentsList = CreateObject<CListWidget>(recentsTab, "RecentsList");

            const Array<String>& recentProjectPaths = projectManager->GetRecentProjectsList();
            this->recentProjectPaths.Clear();

            for (int i = 0; i < recentProjectPaths.GetSize(); ++i)
            {
                IO::Path path = recentProjectPaths[i];

                if (path.Exists() && path.GetExtension().GetString() == projectManager->GetProjectFileExtension())
                {
                    this->recentProjectPaths.Add(path);
                }
            }

            for (int i = this->recentProjectPaths.GetSize() - 1; i >= 0; --i)
            {
                const IO::Path& path = this->recentProjectPaths[i];

                CListWidgetItem* item = CreateObject<CListWidgetItem>(recentsList, "ListWidgetItem");

                CLabel* titleLabel = CreateObject<CLabel>(item, "Title");
                titleLabel->AddStyleClass("Title");
                titleLabel->SetText(path.GetFileName().RemoveExtension().GetString());
                
                CLabel* descLabel = CreateObject<CLabel>(item, "DescLabel");
                descLabel->SetText(path.GetString().Replace({'\\'}, '/'));
            }

            recentsList->onSelectionChanged += FUNCTION_BINDING(this, OnProjectTemplateSelectionChanged);

            CWidget* bottomSection = CreateObject<CWidget>(recentsTab, "BottomSection");

            CWidget* hstack1 = CreateObject<CWidget>(bottomSection, "HStack");
            hstack1->AddStyleClass("HStack");
            {
                CLabel* locationLabel = CreateObject<CLabel>(hstack1, "InputLabel");
                locationLabel->SetText("Project Location");

                CTextInput* input = CreateObject<CTextInput>(hstack1, "InputField");
                input->SetHint("Enter project location...");
                input->SetText(defaultOpenProjectLocation);
                openProjectLocation = input;

                CButton* openFolderBtn = CreateObject<CButton>(hstack1, "OpenFolderBtn");
                openFolderBtn->SetText("...");

                openFolderBtn->onButtonLeftClicked += [input, this]
                    {
                        static const Array<EditorPlatform::FileType> fileTypes = { {.desc = "Crystal Project File", .extensions = { "*.cproject" } } };

                        String newLocation = EditorPlatform::ShowFileSelectionDialog(defaultOpenProjectLocation, fileTypes)
                            .GetString()
                            .Replace({ '\\' }, '/');
                        if (newLocation.IsEmpty())
                            return;

                        defaultOpenProjectLocation = newLocation;
                        input->SetText(defaultOpenProjectLocation);
                    };

                input->onTextEdited += FUNCTION_BINDING(this, ValidateInputFields);

                input->onEditingFinished += FUNCTION_BINDING(this, ValidateInputFields);
            }

            openErrorBox = CreateObject<CWidget>(bottomSection, "ErrorBox");
            {
                CWidget* icon = CreateObject<CWidget>(openErrorBox, "ErrorIcon");
                icon->SetBackgroundImage("/Editor/Assets/Icons/Warning");

                openErrorLabel = CreateObject<CLabel>(openErrorBox, "ErrorLabel");
                openErrorLabel->SetText("Please enter a valid project name");
            }

            openErrorBox->SetVisible(false);

            CWidget* buttonGroup = CreateObject<CWidget>(bottomSection, "ButtonGroup");
            {
                openButton = CreateObject<CButton>(buttonGroup, "PrimaryBtn");
                openButton->SetText("Open");
                openButton->SetAlternateStyle(true);

                openButton->onButtonLeftClicked += FUNCTION_BINDING(this, OpenProject);

                CButton* cancelButton = CreateObject<CButton>(buttonGroup, "CancelBtn");
                cancelButton->SetText("Cancel");

                cancelButton->onButtonLeftClicked += [this]
                    {
                        this->Hide();
                        this->QueueDestroy();
                    };
            }
	    }

        // ---------------------------------
        // - New Project Tab

        CTabWidgetContainer* newTab = CreateObject<CTabWidgetContainer>(tabWidget, "NewTab");
        newTab->SetTitle("New Project");
	    {
            CListWidget* newList = CreateObject<CListWidget>(newTab, "NewList");

            {
                CListWidgetItem* emptyProjectItem = CreateObject<CListWidgetItem>(newList, "ListWidgetItem");

                CLabel* titleLabel = CreateObject<CLabel>(emptyProjectItem, "Title");
                titleLabel->AddStyleClass("Title");
                titleLabel->SetText("Empty project");

                CLabel* descLabel = CreateObject<CLabel>(emptyProjectItem, "DescLabel");
                descLabel->SetText("Create an empty project");
            }

            newList->Select(0);

            newList->onSelectionChanged += FUNCTION_BINDING(this, OnProjectTemplateSelectionChanged);

            CWidget* bottomSection = CreateObject<CWidget>(newTab, "BottomSection");

            CWidget* hstack1 = CreateObject<CWidget>(bottomSection, "HStack");
            hstack1->AddStyleClass("HStack");
            {
                CLabel* locationLabel = CreateObject<CLabel>(hstack1, "InputLabel");
                locationLabel->SetText("Project Location");

                CTextInput* input = CreateObject<CTextInput>(hstack1, "InputField");
                input->SetHint("Enter project folder path...");
                input->SetText(defaultNewProjectLocation);
                newProjectLocation = input;

                CButton* openFolderBtn = CreateObject<CButton>(hstack1, "OpenFolderBtn");
                openFolderBtn->SetText("...");

                openFolderBtn->onButtonLeftClicked += [input, this]
                    {
                        String newLocation = EditorPlatform::ShowSelectDirectoryDialog(defaultNewProjectLocation)
                            .GetString()
                            .Replace({ '\\' }, '/');
                        if (newLocation.IsEmpty())
                            return;

                        defaultNewProjectLocation = newLocation;
                        input->SetText(defaultNewProjectLocation);
                    };

                input->onTextEdited += FUNCTION_BINDING(this, ValidateInputFields);

                input->onEditingFinished += FUNCTION_BINDING(this, ValidateInputFields);
            }

            CWidget* hstack2 = CreateObject<CWidget>(bottomSection, "HStack");
            hstack2->AddStyleClass("HStack");
            {
                CLabel* locationLabel = CreateObject<CLabel>(hstack2, "InputLabel");
                locationLabel->SetText("Project Name");

                CTextInput* input = CreateObject<CTextInput>(hstack2, "InputField");
                input->SetHint("Enter project name...");
                newProjectName = input;

                input->onTextEdited += FUNCTION_BINDING(this, ValidateInputFields);

                input->onEditingFinished += FUNCTION_BINDING(this, ValidateInputFields);
            }

            newErrorBox = CreateObject<CWidget>(bottomSection, "ErrorBox");
            {
                CWidget* icon = CreateObject<CWidget>(newErrorBox, "ErrorIcon");
                icon->SetBackgroundImage("/Editor/Assets/Icons/Warning");

                newErrorLabel = CreateObject<CLabel>(newErrorBox, "ErrorLabel");
                newErrorLabel->SetText("Please enter a valid project name");
            }

            newErrorBox->SetVisible(false);

            CWidget* buttonGroup = CreateObject<CWidget>(bottomSection, "ButtonGroup");
            {
                createButton = CreateObject<CButton>(buttonGroup, "PrimaryBtn");
                createButton->SetText("Create");
                createButton->SetAlternateStyle(true);

                createButton->onButtonLeftClicked += FUNCTION_BINDING(this, CreateProject);
                
                CButton* cancelButton = CreateObject<CButton>(buttonGroup, "CancelBtn");
                cancelButton->SetText("Cancel");

                cancelButton->onButtonLeftClicked += [this]
                    {
                        this->Hide();
                        this->QueueDestroy();
                    };
            }
	    }

        if (this->recentProjectPaths.IsEmpty())
        {
            tabWidget->SetActiveTab(1);
        }

        ValidateInputFields(nullptr);
    }

    void ProjectBrowser::ValidateInputFields(FTextInput*)
    {
        static const HashSet<CE::Name> reservedNames = {
            "Game", "Engine", "Editor", "Plugin"
        };

        if (tabWidget->GetActiveTabIndex() == 0)
        {
            defer(
                openButton->SetInteractable(isValidInput);
				openErrorBox->SetVisible(!isValidInput);
            );

            // Recent Projects tab
            IO::Path location = openProjectLocation->GetText();
            if (location.Exists() && location.IsDirectory())
            {
                isValidInput = false;
                openErrorLabel->SetText("Please enter valid project location.");
                return;
            }

            if (location.GetExtension() != ProjectManager::Get()->GetProjectFileExtension())
            {
                isValidInput = false;
                openErrorLabel->SetText("Please enter valid project location.");
	            return;
            }

            isValidInput = true;
        }
        else // New Project tab
        {
            defer(
                createButton->SetInteractable(isValidInput);
				newErrorBox->SetVisible(!isValidInput);
            );

            IO::Path location = newProjectLocation->GetText();
            if (location.Exists() && !location.IsDirectory())
            {
                isValidInput = false;
                newErrorLabel->SetText("The project location entered is not a valid directory");
	            return;
            }

            projectName = newProjectName->GetText();
            if (!IsValidObjectName(projectName) || projectName.GetLength() > 24)
            {
                isValidInput = false;
                newErrorLabel->SetText("The project name should be under 24 letters and should not have special characters except underscore (_).");
	            return;
            }

            if ((location / projectName).Exists())
            {
                isValidInput = false;
                newErrorLabel->SetText("The project with given name already exists at specified location.");
                return;
            }

            if (reservedNames.Exists(projectName))
            {
                isValidInput = false;
                newErrorLabel->SetText(String::Format("The project name {} is reserved. Please use something else.", projectName));
	            return;
            }
            
            isValidInput = true;
        }
    }

    void ProjectBrowser::OnProjectTemplateSelectionChanged()
    {
        
    }

    void ProjectBrowser::OnRecentProjectSelectionChanged()
    {
        const auto& selection = recentsList->GetSelection();
        if (selection.IsEmpty())
            return;


    }

    void ProjectBrowser::CreateProject()
    {
        ValidateInputFields(newProjectLocation);

        if (isValidInput)
        {
            IO::Path location = newProjectLocation->GetText();
            defaultNewProjectLocation = location.GetString();
            if (!location.Exists())
            {
                IO::Path::CreateDirectories(location);
            }
            ProjectManager::Get()->CreateEmptyProject(location / projectName, projectName);

            gExitLaunchProcess = PlatformDirectories::GetLaunchDir() / "EditorLauncher";
            gExitLaunchArguments = location / projectName;

            this->QueueDestroy();
        }
        else
        {
            createButton->SetInteractable(false);
        }
    }

    void ProjectBrowser::OpenProject()
    {
        ValidateInputFields(openProjectLocation);

        if (isValidInput)
        {
            IO::Path location = openProjectLocation->GetText();
            defaultOpenProjectLocation = location.GetString();

            // TODO: Pass project path as argument

            gExitLaunchProcess = PlatformDirectories::GetLaunchDir() / "EditorLauncher";
            gExitLaunchArguments = defaultOpenProjectLocation;

            this->QueueDestroy();
        }
        else
        {
            openButton->SetInteractable(false);
        }
    }

} // namespace CE::Editor
