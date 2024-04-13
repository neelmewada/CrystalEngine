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

        CWidget* mainSection = CreateObject<CWidget>(this, "MainSection");

        CListWidget* templateList = CreateObject<CListWidget>(this, "ProjectTemplateList");

        for (int i = 0; i < 1; ++i)
        {
            CListWidgetItem* templateItem = CreateObject<CListWidgetItem>(templateList, "ProjectTemplate");

            CLabel* titleLabel = CreateObject<CLabel>(templateItem, "TitleLabel");
            titleLabel->SetText("Empty Project");

            CLabel* descLabel = CreateObject<CLabel>(templateItem, "DescLabel");
            descLabel->SetText("Create an empty project!");
        }

        templateList->Select(0);

        Object::Bind(templateList, MEMBER_FUNCTION(CListWidget, OnSelectionChanged),
            this, MEMBER_FUNCTION(Self, OnProjectTemplateSelectionChanged));
    }

    void ProjectBrowser::OnProjectTemplateSelectionChanged()
    {

    }

} // namespace CE::Editor
