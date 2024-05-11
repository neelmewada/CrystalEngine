#include "CrystalEditor.h"

namespace CE::Editor
{

    CrystalEditorWindow::CrystalEditorWindow()
    {
        dockType = CDockType::Major;
    }

    CrystalEditorWindow::~CrystalEditorWindow()
    {
	    
    }

    void CrystalEditorWindow::Construct()
    {
	    Super::Construct();

        SetTitle("Crystal Editor");

        CDockSplitView* root = GetRootDockSplit();
      
        sceneEditor = CreateObject<SceneEditorWindow>(root, "SceneEditor");
        viewportWindow = sceneEditor->GetViewportWindow();
        EditorViewport* editorViewport = viewportWindow->GetViewport();

        sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();
        sceneSubsystem->SetMainViewport(editorViewport);

        rendererSubsystem = gEngine->GetSubsystem<RendererSubsystem>();

        CE::Scene* scene = sceneSubsystem->GetMainScene();

        sceneEditor->GetSceneHierarchyWindow()->SetScene(scene);

        // -------------------------------------
        // TODO: Sample scene & test code here

        auto assetManager = gEngine->GetAssetManager();

        CTimer* timer = CreateObject<CTimer>(this, "TickTimer");
        Bind(timer, MEMBER_FUNCTION(CTimer, OnTimeOut),
            this, MEMBER_FUNCTION(Self, TimerTick));
        timer->Start(10);

        CE::Shader* standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
        CE::Shader* skyboxShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/SkyboxCubeMap");
        CE::Shader* iblConvolutionShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/IBLConvolution");
        CE::TextureCube* skybox = assetManager->LoadAssetAtPath<CE::TextureCube>("/Engine/Assets/Textures/HDRI/sample_night2");

        CE::Texture* albedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/albedo");
        CE::Texture* normalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/normal");
        CE::Texture* metallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/metallic");
        CE::Texture* roughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/roughness");

        scene->SetSkyboxCubeMap(skybox);

	    {
            StaticMeshActor* meshActor = CreateObject<StaticMeshActor>(scene, "Sphere_0");
            scene->AddActor(meshActor);

            meshComponent = meshActor->GetMeshComponent();

            StaticMesh* cubeMesh = CreateObject<StaticMesh>(scene, "StaticMesh");
            {
                RPI::ModelAsset* cubeModel = CreateObject<ModelAsset>(cubeMesh, "CubeModel");
                RPI::ModelLodAsset* cubeLodAsset = RPI::ModelLodAsset::CreateCubeAsset(cubeModel);
                cubeModel->AddModelLod(cubeLodAsset);

                cubeMesh->SetModelAsset(cubeModel);
            }
            meshComponent->SetStaticMesh(cubeMesh);

            meshComponent->SetLocalPosition(Vec3(0, 0, 10));
            meshComponent->SetLocalEulerAngles(Vec3(0, 0, 0));

            StaticMesh* sphereMesh = CreateObject<StaticMesh>(scene, "SphereMesh");
            {
                RPI::ModelAsset* sphereModel = CreateObject<ModelAsset>(sphereMesh, "SphereModel");
                RPI::ModelLodAsset* sphereLodAsset = RPI::ModelLodAsset::CreateSphereAsset(sphereModel);
                sphereModel->AddModelLod(sphereLodAsset);

                sphereMesh->SetModelAsset(sphereModel);
            }
            meshComponent->SetStaticMesh(sphereMesh);

            CE::Material* material = CreateObject<CE::Material>(meshComponent, "Material");
            material->SetShader(standardShader);
            meshComponent->SetMaterial(material, 0, 0);

            material->SetProperty("_AlbedoTex", albedoTex);
            material->SetProperty("_NormalTex", normalTex);
            material->SetProperty("_MetallicTex", metallicTex);
            material->SetProperty("_RoughnessTex", roughnessTex);
            material->ApplyProperties();

            {
                Vec3 cubePositions[4] = { Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(-1, 0, 0), Vec3(0, -1, 0) };

	            for (int i = 0; i < 4; ++i)
	            {
                    StaticMeshActor* cubeActor = CreateObject<StaticMeshActor>(meshActor, String::Format("Cube_{}", i));
                    meshActor->AttachActor(cubeActor);

                    StaticMeshComponent* cubeMeshComponent = cubeActor->GetMeshComponent();
                    cubeMeshComponent->SetStaticMesh(cubeMesh);

                    cubeMeshComponent->SetLocalPosition(cubePositions[i] * 1.5f);

                    cubeMeshComponent->SetMaterial(material, 0);
	            }
            }

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

            CameraComponent* cameraComponent = CreateObject<CameraComponent>(meshComponent, "Camera");
            cameraComponent->SetLocalPosition(Vec3(0, 0, -2));
            cameraComponent->SetFarPlane(500);
            meshComponent->SetupAttachment(cameraComponent);
	    }
    }

    void CrystalEditorWindow::TimerTick()
    {
        auto curTime = clock();
        const f32 deltaTime = (f32)(curTime - prevClock) / CLOCKS_PER_SEC;

        rotation += 30 * deltaTime;
        meshComponent->SetLocalEulerAngles(Vec3(0, 0, rotation));

        prevClock = curTime;
    }

    void CrystalEditorWindow::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (sceneSubsystem && sceneSubsystem->GetMainViewport() == viewportWindow->GetViewport())
        {
            sceneSubsystem->SetMainViewport(nullptr);
        }
    }

} // namespace CE::Editor
