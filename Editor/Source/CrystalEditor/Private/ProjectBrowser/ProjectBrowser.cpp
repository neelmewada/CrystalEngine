#include "CrystalEditor.h"

namespace CE::Editor
{

    ProjectBrowser::ProjectBrowser()
    {
        title = "Crystal Project Browser";
    }

    ProjectBrowser::~ProjectBrowser()
    {
	    
    }

    void ProjectBrowser::Construct()
    {
	    Super::Construct();

        LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/ProjectBrowserStyle.css");

        tabWidget = CreateObject<CTabWidget>(this, "TabWidget");
        
        ProjectManager* projectManager = ProjectManager::Get();

        CTabWidgetContainer* recentsTab = CreateObject<CTabWidgetContainer>(tabWidget, "RecentsTab");
        recentsTab->SetTitle("Recent Projects");
	    {
            CListWidget* recentsList = CreateObject<CListWidget>(recentsTab, "RecentsList");

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

            CWidget* bottomSection = CreateObject<CWidget>(recentsTab, "BottomSection");
            
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

            Object::Bind(newList, MEMBER_FUNCTION(CListWidget, OnSelectionChanged),
                this, MEMBER_FUNCTION(Self, OnProjectTemplateSelectionChanged));

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

                Bind(openFolderBtn, MEMBER_FUNCTION(CButton, OnButtonLeftClicked), [input, this]
                    {
                        String newLocation = EditorPlatform::ShowSelectDirectoryDialog(defaultNewProjectLocation)
                			.GetString()
                			.Replace({'\\'}, '/');
                        if (newLocation.IsEmpty())
                            return;
						
                        defaultNewProjectLocation = newLocation;
                        input->SetText(defaultNewProjectLocation);
                    });

                Bind(input, MEMBER_FUNCTION(CTextInput, OnTextChanged),
                    this, MEMBER_FUNCTION(Self, ValidateInputFields));

                Bind(input, MEMBER_FUNCTION(CTextInput, OnEditingFinished),
                    this, MEMBER_FUNCTION(Self, ValidateInputFields));
            }

            CWidget* hstack2 = CreateObject<CWidget>(bottomSection, "HStack");
            hstack2->AddStyleClass("HStack");
            {
                CLabel* locationLabel = CreateObject<CLabel>(hstack2, "InputLabel");
                locationLabel->SetText("Project Name");

                CTextInput* input = CreateObject<CTextInput>(hstack2, "InputField");
                input->SetHint("Enter project name...");
                newProjectName = input;

                Bind(input, MEMBER_FUNCTION(CTextInput, OnTextChanged),
                    this, MEMBER_FUNCTION(Self, ValidateInputFields));

                Bind(input, MEMBER_FUNCTION(CTextInput, OnEditingFinished),
                    this, MEMBER_FUNCTION(Self, ValidateInputFields));
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

                Bind(createButton, MEMBER_FUNCTION(CButton, OnButtonLeftClicked),
                    this, MEMBER_FUNCTION(Self, CreateProject));
                
                CButton* cancelButton = CreateObject<CButton>(buttonGroup, "CancelBtn");
                cancelButton->SetText("Cancel");

                Bind(cancelButton, MEMBER_FUNCTION(CButton, OnButtonLeftClicked), [this]
                    {
                        this->Hide();
                        this->QueueDestroy();
                    });
            }
	    }

        if (this->recentProjectPaths.IsEmpty())
        {
            tabWidget->SetActiveTab(1);
        }

        ValidateInputFields(nullptr);
    }

    void ProjectBrowser::ValidateInputFields(CTextInput*)
    {
        if (tabWidget->GetActiveTabIndex() == 0)
        {
            // Recent Projects tab
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
            
            isValidInput = true;
        }
    }

    void ProjectBrowser::OnProjectTemplateSelectionChanged()
    {
        
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
            ProjectManager::Get()->CreateEmptyProject(location, projectName);

            gExitLaunchProcess = PlatformDirectories::GetLaunchDir() / "EditorLauncher";
            gExitLaunchArguments = "";

            this->QueueDestroy();
        }
        else
        {
            createButton->SetInteractable(false);
        }
    }

} // namespace CE::Editor
