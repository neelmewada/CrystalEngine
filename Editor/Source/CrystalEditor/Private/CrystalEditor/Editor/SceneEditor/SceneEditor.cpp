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

        Ref<TextureCube> skybox = assetManager->LoadAssetAtPath<TextureCube>("/Engine/Assets/Textures/HDRI/sample_night");
        Ref<CE::Shader> standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
        Ref<CE::Shader> skyboxShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/SkyboxCubeMap");

    	Ref<CE::Texture> albedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/albedo");
        Ref<CE::Texture> normalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/normal");
        Ref<CE::Texture> metallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/metallic");
        Ref<CE::Texture> roughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/roughness");

    	Ref<CE::Texture> plasticAlbedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/albedo");
        Ref<CE::Texture> plasticNormalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/normal");
        Ref<CE::Texture> plasticMetallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/metallic");
        Ref<CE::Texture> plasticRoughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/roughness");

    	Ref<CE::Texture> woodAlbedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/albedo");
        Ref<CE::Texture> woodNormalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/normal");
        Ref<CE::Texture> woodMetallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/metallic");
        Ref<CE::Texture> woodRoughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/roughness");


        scene->SetSkyboxCubeMap(skybox.Get());

        {
            CE::Material* material = CreateObject<CE::Material>(scene, "Material");
            material->SetShader(standardShader.Get());

            material->SetProperty("_AlbedoTex", albedoTex);
            material->SetProperty("_NormalTex", normalTex);
            material->SetProperty("_MetallicTex", metallicTex);
            material->SetProperty("_RoughnessTex", roughnessTex);
            material->ApplyProperties();

            CE::Material* plasticMaterial = CreateObject<CE::Material>(scene, "PlasticMaterial");
            plasticMaterial->SetShader(standardShader.Get());
	        {
		        plasticMaterial->SetProperty("_AlbedoTex", plasticAlbedoTex);
            	plasticMaterial->SetProperty("_NormalTex", plasticNormalTex);
            	plasticMaterial->SetProperty("_MetallicTex", plasticMetallicTex);
            	plasticMaterial->SetProperty("_RoughnessTex", plasticRoughnessTex);
            	plasticMaterial->ApplyProperties();
	        }

            CE::Material* woodMaterial = CreateObject<CE::Material>(scene, "WoodMaterial");
            woodMaterial->SetShader(standardShader.Get());
        	{
                woodMaterial->SetProperty("_AlbedoTex", woodAlbedoTex);
                woodMaterial->SetProperty("_NormalTex", woodNormalTex);
                woodMaterial->SetProperty("_MetallicTex", woodMetallicTex);
                woodMaterial->SetProperty("_RoughnessTex", woodRoughnessTex);
                woodMaterial->ApplyProperties();
            }

            StaticMesh* sphereMesh = CreateObject<StaticMesh>(scene, "SphereMesh");
            {
                RPI::ModelAsset* sphereModel = CreateObject<ModelAsset>(sphereMesh, "SphereModel");
                RPI::ModelLodAsset* sphereLodAsset = RPI::ModelLodAsset::CreateSphereAsset(sphereModel);
                sphereModel->AddModelLod(sphereLodAsset);

                sphereMesh->SetModelAsset(sphereModel);
            }

            StaticMesh* cubeMesh = CreateObject<StaticMesh>(scene, "CubeMesh");
            {
                RPI::ModelAsset* cubeModel = CreateObject<ModelAsset>(cubeMesh, "CubeModel");
                RPI::ModelLodAsset* cubeLodAsset = RPI::ModelLodAsset::CreateCubeAsset(cubeModel);
                cubeModel->AddModelLod(cubeLodAsset);

                cubeMesh->SetModelAsset(cubeModel);
            }

            StaticMeshActor* sphereActor = CreateObject<StaticMeshActor>(scene, "SphereMesh");
            scene->AddActor(sphereActor);
	        {
		        StaticMeshComponent* meshComponent = sphereActor->GetMeshComponent();
            	meshComponent->SetStaticMesh(sphereMesh);
            	meshComponent->SetLocalPosition(Vec3(0, 0, 5));
            	meshComponent->SetLocalEulerAngles(Vec3(0, 0, 0));
            	meshComponent->SetMaterial(material, 0, 0);
	        }

            StaticMeshActor* groundActor = CreateObject<StaticMeshActor>(scene, "Ground");
            scene->AddActor(groundActor);
            {
                StaticMeshComponent* meshComponent = groundActor->GetMeshComponent();
                meshComponent->SetStaticMesh(cubeMesh);
                meshComponent->SetLocalPosition(Vec3(0, -1, 5));
                meshComponent->SetLocalScale(Vec3(10, 0.1f, 10));
                meshComponent->SetMaterial(woodMaterial, 0, 0);
            }

            DirectionalLight* lightActor = CreateObject<DirectionalLight>(scene, "Sun");
            scene->AddActor(lightActor);
            {
                DirectionalLightComponent* directionalLight = lightActor->GetDirectionalLightComponent();
                directionalLight->SetLocalEulerAngles(Vec3(30, 0, 0));
                directionalLight->SetIntensity(10);
                directionalLight->SetLightColor(Color::White());
            }

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
                skyboxMaterial->SetShader(skyboxShader.Get());
                skyboxMeshComponent->SetMaterial(skyboxMaterial, 0, 0);

                skyboxMaterial->SetProperty("_CubeMap", skybox);
                skyboxMaterial->ApplyProperties();
            }

            Actor* sampleActor = CreateObject<Actor>(scene, "SampleActor");
            {
                SceneComponent* root = CreateObject<SceneComponent>(sampleActor, "Root");
                sampleActor->SetRootComponent(root);

                for (int i = 0; i < 4; ++i)
                {
                    SceneComponent* child = CreateObject<SceneComponent>(root, String::Format("Child_{}", i));
                    root->SetupAttachment(child);
                }

                scene->AddActor(sampleActor);
            }
        }

        gEditor->AddRenderViewport(viewport);
        gEngine->LoadScene(scene);

        Title(scene->GetName().GetString());
    }

    void SceneEditor::Construct()
    {
        Super::Construct();

        Title("Scene Editor");

        gEngine->GetSceneSubsystem()->AddCallbacks(this);

        ConstructMenuBar();
        ConstructDockspaces();

        LoadSandboxScene();
    }

    void SceneEditor::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        if (SceneSubsystem* sceneSubsystem = gEngine->GetSceneSubsystem())
        {
            sceneSubsystem->RemoveCallbacks(this);
        }
    }

    void SceneEditor::OnSceneLoaded(CE::Scene* scene)
    {
        sceneOutlinerTab->SetScene(scene);
    }

    void SceneEditor::OnSelectionChanged(FItemSelectionModel* selectionModel)
    {
        const auto& selection = selectionModel->GetSelection();

        Array<Actor*> selectedActors{};

        for (const FModelIndex& modelIndex : selection)
        {
            if (!modelIndex.IsValid())
                continue;

            Actor* actor = (Actor*)modelIndex.GetDataPtr();
            selectedActors.Add(actor);
        }

        if (selectedActors.NotEmpty())
        {
            // TODO: Create and set object editor
            detailsTab->SetSelectedActor(selectedActors.GetLast());
        }
        else
        {
            detailsTab->SetSelectedActor(nullptr);
        }
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
                        .FillRatio(0.3f),

                        FAssignNew(EditorMinorDockspace, rightBottom)
                        .DockTabs(
                            FAssignNew(DetailsTab, detailsTab)

                        )
                        .HAlign(HAlign::Fill)
                        .FillRatio(0.7f)
                    )
                )
            )
    		.Padding(Vec4(0, 5, 0, 0));

        sceneOutlinerTab->treeView->SelectionModel()->OnSelectionChanged(FUNCTION_BINDING(this, OnSelectionChanged));
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
                        .Text("Project Settings")
                        .OnClick([this]
                        {
                            ProjectSettingsEditor::Show();
                        }),

                        FNew(FMenuItem)
                        .Text("Editor Settings")
                    )
                )
                .Name("EditMenuItem"),

                FNew(FMenuItem)
                .Text("Help")
                .SubMenu(
					FNew(EditorMenuPopup)
                    .Name("HelpMenu")
                    .As<EditorMenuPopup>()
                    .Gap(0)
                    .Content(
                        FNew(FMenuItem)
                        .Text("Fusion Samples")
                        .OnClick([this]
                        {
                            SampleWidgetWindow::Show();
                        }),

						FNew(FMenuItem)
                        .Text("About")
                        .OnClick([this]
                        {
                            AboutWindow::Show();
                        })
                    )
                )
                .Name("HelpMenuItem")
            )
            ;
    }

}

