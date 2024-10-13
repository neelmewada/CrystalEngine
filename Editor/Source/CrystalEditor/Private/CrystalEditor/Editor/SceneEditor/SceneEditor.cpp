#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneEditor::SceneEditor()
    {

    }

    void SceneEditor::LoadSandboxScene()
    {
        CE::Scene* sandbox = CreateObject<CE::Scene>(this, "SandboxScene");

        EditorViewport* viewport = viewportTab->GetViewport();
        viewport->SetScene(sandbox->GetRpiScene());

        TextureCube* skybox = gEngine->GetAssetManager()->LoadAssetAtPath<TextureCube>("/Engine/Assets/Textures/HDRI/sample_night");
        sandbox->SetSkyboxCubeMap(skybox);

        gEditor->AddRenderViewport(viewport);
        gEngine->LoadScene(sandbox);
    }

    void SceneEditor::Construct()
    {
        Super::Construct();

        Title("Scene Editor");

        ConstructMenuBar();
        ConstructDockspaces();

        LoadSandboxScene();
    }

    void SceneEditor::ConstructDockspaces()
    {

        (*content)
            .Child(
                FAssignNew(FSplitBox, rootSplitBox)
                .Direction(FSplitDirection::Horizontal)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                (
                    FNew(FSplitBox)
                    .Direction(FSplitDirection::Vertical)
                    .VAlign(VAlign::Fill)
                    .FillRatio(0.75f)
                    (
                        FAssignNew(EditorMinorDockspace, center)
                        .DockTabs(
                            FAssignNew(EditorViewportTab, viewportTab)
                            
                        )
                        .HAlign(HAlign::Fill)
                        .FillRatio(0.6f),

                        FAssignNew(EditorMinorDockspace, bottom)
                        .DockTabs(
                            FNew(EditorMinorDockTab)
                            .Title("Logs")

                        )
                        .HAlign(HAlign::Fill)
                        .FillRatio(0.4f)
                    ),

                    FAssignNew(EditorMinorDockspace, right)
                    .DockTabs(
                        FNew(EditorMinorDockTab)
                        .Title("Details")

                    )
                    .VAlign(VAlign::Fill)
                    .FillRatio(0.25f)
                )
            )
    		.Padding(Vec4(0, 5, 0, 0));
    }

    void SceneEditor::ConstructMenuBar()
    {
        (*menuBar)
            .Content(
                FNew(FMenuItem)
                .Text("File")
                .SubMenu(
                    FNew(EditorMenuPopup)
                    .Name("FileMenu")
                    .As<EditorMenuPopup>()
                    .Gap(0)
                    .Content(
                        FNew(FMenuItem)
                        .Text("New"),

                        FNew(FMenuItem)
                        .Text("Open"),

                        FNew(FMenuItem)
                        .Text("Open As..."),

                        FNew(FMenuItem)
                        .Text("Open Recent...")
                        .SubMenu(
                            FNew(EditorMenuPopup)
                            .Gap(0)
                            .Content(
                                FNew(FMenuItem)
                                .Text("Project 1"),

                                FNew(FMenuItem)
                                .Text("Project 2"),

                                FNew(FMenuItem)
                                .Text("Project 3"),

                                FNew(FMenuItem)
                                .Text("Project 4")
                            )
                            .Name("RecentsMenu")
                            .As<EditorMenuPopup>()
                        ),

                        FNew(FMenuItem)
                        .Text("Save"),

                        FNew(FMenuItem)
                        .Text("Save As..."),

                        FNew(FMenuItem)
                        .Text("Exit")
                        .OnClick([this]
                            {
                                GetContext()->QueueDestroy();
                            })
                    )
                )
                .Name("FileMenuItem"),

                FNew(FMenuItem)
                .Text("Edit")
                .SubMenu(
                    FNew(EditorMenuPopup)
                    .Name("EditMenu")
                    .As<EditorMenuPopup>()
                    .Gap(0)
                    .Content(
                        FNew(FMenuItem)
                        .Text("Project Settings"),

                        FNew(FMenuItem)
                        .Text("Editor Settings")
                    )
                )
                .Name("EditMenuItem"),

                FNew(FMenuItem)
                .Text("Help")
                .Name("HelpMenuItem")
            )
            ;
    }

}

