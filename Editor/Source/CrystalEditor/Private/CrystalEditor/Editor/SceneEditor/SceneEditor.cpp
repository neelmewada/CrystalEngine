#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneEditor::SceneEditor()
    {

    }

    void SceneEditor::LoadSandboxScene()
    {
        CE::Scene* scene = CreateObject<CE::Scene>(this, "SandboxScene");

        EditorViewport* viewport = viewportTab->GetViewport();
        viewport->SetScene(scene->GetRpiScene());

        auto assetManager = gEngine->GetAssetManager();

        TextureCube* skybox = assetManager->LoadAssetAtPath<TextureCube>("/Engine/Assets/Textures/HDRI/sample_night");
        CE::Shader* standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
        CE::Shader* skyboxShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/SkyboxCubeMap");

        CE::Texture* albedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/albedo");
        CE::Texture* normalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/normal");
        CE::Texture* metallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/metallic");
        CE::Texture* roughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/roughness");

        CE::Texture* plasticAlbedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/albedo");
        CE::Texture* plasticNormalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/normal");
        CE::Texture* plasticMetallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/metallic");
        CE::Texture* plasticRoughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/roughness");

        scene->SetSkyboxCubeMap(skybox);

        {
            CE::Material* material = CreateObject<CE::Material>(scene, "Material");
            material->SetShader(standardShader);

            material->SetProperty("_AlbedoTex", albedoTex);
            material->SetProperty("_NormalTex", normalTex);
            material->SetProperty("_MetallicTex", metallicTex);
            material->SetProperty("_RoughnessTex", roughnessTex);
            material->ApplyProperties();

            CE::Material* plasticMaterial = CreateObject<CE::Material>(scene, "PlasticMaterial");
            plasticMaterial->SetShader(standardShader);

            plasticMaterial->SetProperty("_AlbedoTex", plasticAlbedoTex);
            plasticMaterial->SetProperty("_NormalTex", plasticNormalTex);
            plasticMaterial->SetProperty("_MetallicTex", plasticMetallicTex);
            plasticMaterial->SetProperty("_RoughnessTex", plasticRoughnessTex);
            plasticMaterial->ApplyProperties();

            StaticMesh* sphereMesh = CreateObject<StaticMesh>(scene, "SphereMesh");
            {
                RPI::ModelAsset* sphereModel = CreateObject<ModelAsset>(sphereMesh, "SphereModel");
                RPI::ModelLodAsset* sphereLodAsset = RPI::ModelLodAsset::CreateSphereAsset(sphereModel);
                sphereModel->AddModelLod(sphereLodAsset);

                sphereMesh->SetModelAsset(sphereModel);
            }

            StaticMeshActor* meshActor = CreateObject<StaticMeshActor>(scene, "SampleMesh");
            scene->AddActor(meshActor);

            StaticMeshComponent* meshComponent = meshActor->GetMeshComponent();
            meshComponent->SetStaticMesh(sphereMesh);
            meshComponent->SetLocalPosition(Vec3(0, 0, 5));
            meshComponent->SetLocalEulerAngles(Vec3(0, 0, 0));
            meshComponent->SetMaterial(material, 0, 0);

            CameraActor* camera = CreateObject<CameraActor>(scene, "Camera");
            camera->GetCameraComponent()->SetLocalPosition(Vec3(0, 0, 0));
            scene->AddActor(camera);

            CameraComponent* cameraComponent = camera->GetCameraComponent();
            cameraComponent->SetFieldOfView(60);

            StaticMeshActor* skyboxActor = CreateObject<StaticMeshActor>(scene, "SkyboxActor");
            scene->AddActor(skyboxActor);

            StaticMeshComponent* skyboxMeshComponent = skyboxActor->GetMeshComponent();
            skyboxMeshComponent->SetStaticMesh(sphereMesh);

            skyboxMeshComponent->SetLocalPosition(Vec3(0, 0, 0));
            skyboxMeshComponent->SetLocalScale(Vec3(1, 1, 1) * 1000);

            {
                CE::Material* skyboxMaterial = CreateObject<CE::Material>(skyboxMeshComponent, "Material");
                skyboxMaterial->SetShader(skyboxShader);
                skyboxMeshComponent->SetMaterial(skyboxMaterial, 0, 0);

                skyboxMaterial->SetProperty("_CubeMap", skybox);
                skyboxMaterial->ApplyProperties();
            }
        }

        gEditor->AddRenderViewport(viewport);
        gEngine->LoadScene(scene);
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

                    FNew(FSplitBox)
                    .Direction(FSplitDirection::Vertical)
                    .VAlign(VAlign::Fill)
                    .FillRatio(0.25f)
                    (
                        FAssignNew(EditorMinorDockspace, rightTop)
                        .DockTabs(
                            FAssignNew(SceneOutlinerTab, sceneOutlinerTab)

                        )
                        .HAlign(HAlign::Fill)
                        .FillRatio(0.5f),

                        FAssignNew(EditorMinorDockspace, rightBottom)
                        .DockTabs(
                            FNew(EditorMinorDockTab)
                            .Title("Details")

                        )
                        .HAlign(HAlign::Fill)
                        .FillRatio(0.5f)
                    )
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
