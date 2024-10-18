#include "GameLauncher.h"


void GameLoop::SetupTestScene()
{
	AssetManager* assetManager = AssetManager::Get();

	TextureCube* skybox = assetManager->LoadAssetAtPath<TextureCube>("/Engine/Assets/Textures/HDRI/sample_night");
	CE::Shader* standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
	CE::Shader* skyboxShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/SkyboxCubeMap");

	CE::Scene* scene = CreateObject<CE::Scene>(gEngine, "TestScene");

	GameViewportSubsystem* viewportSubsystem = gEngine->GetSubsystem<GameViewportSubsystem>();
	gameWindow = viewportSubsystem->GetGameWindow();

	gameWindow->SetScene(scene->GetRpiScene());
	scene->SetSkyboxCubeMap(skybox);

	{
		// - Textures & Materials -

		CE::Texture* aluminiumAlbedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/albedo");
		CE::Texture* aluminiumNormalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/normal");
		CE::Texture* aluminiumMetallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/metallic");
		CE::Texture* aluminiumRoughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/roughness");

		CE::Texture* plasticAlbedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/albedo");
		CE::Texture* plasticNormalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/normal");
		CE::Texture* plasticMetallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/metallic");
		CE::Texture* plasticRoughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/roughness");

		CE::Texture* woodAlbedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/albedo");
		CE::Texture* woodNormalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/normal");
		CE::Texture* woodMetallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/metallic");
		CE::Texture* woodRoughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/WoodFloor/roughness");

		CE::Material* aluminiumMaterial = CreateObject<CE::Material>(scene, "Material");
		aluminiumMaterial->SetShader(standardShader);
		{
			aluminiumMaterial->SetProperty("_AlbedoTex", aluminiumAlbedoTex);
			aluminiumMaterial->SetProperty("_NormalTex", aluminiumNormalTex);
			aluminiumMaterial->SetProperty("_MetallicTex", aluminiumMetallicTex);
			aluminiumMaterial->SetProperty("_RoughnessTex", aluminiumRoughnessTex);
			aluminiumMaterial->ApplyProperties();
		}

		CE::Material* plasticMaterial = CreateObject<CE::Material>(scene, "PlasticMaterial");
		plasticMaterial->SetShader(standardShader);
		{
			plasticMaterial->SetProperty("_AlbedoTex", plasticAlbedoTex);
			plasticMaterial->SetProperty("_NormalTex", plasticNormalTex);
			plasticMaterial->SetProperty("_MetallicTex", plasticMetallicTex);
			plasticMaterial->SetProperty("_RoughnessTex", plasticRoughnessTex);
			plasticMaterial->ApplyProperties();
		}

		CE::Material* woodMaterial = CreateObject<CE::Material>(scene, "WoodMaterial");
		woodMaterial->SetShader(standardShader);
		{
			woodMaterial->SetProperty("_AlbedoTex", woodAlbedoTex);
			woodMaterial->SetProperty("_NormalTex", woodNormalTex);
			woodMaterial->SetProperty("_MetallicTex", woodMetallicTex);
			woodMaterial->SetProperty("_RoughnessTex", woodRoughnessTex);
			woodMaterial->ApplyProperties();
		}

		StaticMesh* sphereMesh = CreateObject<StaticMesh>(scene, "SphereMesh");
		{
			RPI::ModelAsset* sphereModel = CreateObject<RPI::ModelAsset>(sphereMesh, "SphereModel");
			RPI::ModelLodAsset* sphereLodAsset = RPI::ModelLodAsset::CreateSphereAsset(sphereModel);
			sphereModel->AddModelLod(sphereLodAsset);

			sphereMesh->SetModelAsset(sphereModel);
		}

		StaticMesh* cubeMesh = CreateObject<StaticMesh>(scene, "CubeMesh");
		{
			RPI::ModelAsset* cubeModel = CreateObject<RPI::ModelAsset>(cubeMesh, "CubeModel");
			RPI::ModelLodAsset* cubeLodAsset = RPI::ModelLodAsset::CreateCubeAsset(cubeModel);
			cubeModel->AddModelLod(cubeLodAsset);

			cubeMesh->SetModelAsset(cubeModel);
		}

		// - Camera -

		CameraActor* camera = CreateObject<CameraActor>(scene, "Camera");
		camera->GetCameraComponent()->SetLocalPosition(Vec3(0, 0, 0));
		scene->AddActor(camera);

		CameraComponent* cameraComponent = camera->GetCameraComponent();
		cameraComponent->SetFieldOfView(60);

		// - Skybox -

		StaticMeshActor* skyboxActor = CreateObject<StaticMeshActor>(scene, "SkyboxActor");
		scene->AddActor(skyboxActor);
		{
			StaticMeshComponent* skyboxMeshComponent = skyboxActor->GetMeshComponent();
			skyboxMeshComponent->SetStaticMesh(sphereMesh);

			skyboxMeshComponent->SetLocalPosition(Vec3(0, 0, 0));
			skyboxMeshComponent->SetLocalScale(Vec3(1, 1, 1) * 1000);

			CE::Material* skyboxMaterial = CreateObject<CE::Material>(skyboxMeshComponent, "Material");
			skyboxMaterial->SetShader(skyboxShader);
			skyboxMeshComponent->SetMaterial(skyboxMaterial, 0, 0);

			skyboxMaterial->SetProperty("_CubeMap", skybox);
			skyboxMaterial->ApplyProperties();
		}

		// - Mesh 1 -

		StaticMeshActor* mesh1 = CreateObject<StaticMeshActor>(scene, "Mesh_1");
		scene->AddActor(mesh1);
		{
			StaticMeshComponent* meshComponent = mesh1->GetMeshComponent();
			meshComponent->SetStaticMesh(sphereMesh);
			meshComponent->SetLocalPosition(Vec3(0, 0, 5));
			meshComponent->SetMaterial(aluminiumMaterial, 0, 0);
		}

		// - Ground -

		StaticMeshActor* groundMesh = CreateObject<StaticMeshActor>(scene, "GroundPlane");
		scene->AddActor(groundMesh);
		{
			StaticMeshComponent* meshComponent = groundMesh->GetMeshComponent();
			meshComponent->SetStaticMesh(cubeMesh);
			meshComponent->SetLocalPosition(Vec3(0, -0.6f, 5));
			meshComponent->SetLocalScale(Vec3(5, 0.05f, 5));
			meshComponent->SetMaterial(woodMaterial, 0, 0);
		}
	}

	gEngine->AddRenderViewport(gameWindow);
	gEngine->LoadScene(scene);
}

