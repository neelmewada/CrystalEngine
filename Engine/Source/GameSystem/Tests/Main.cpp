#include "Core.h"

#include "GameSystem.h"
#include "VulkanRHI.h"

#include <gtest/gtest.h>

using namespace CE;

#include "TestFeatureProcessor.h"

#include "GameSystem_Test.private.h"

using namespace CE;

#define TEST_BEGIN TestBegin(false)
#define TEST_END TestEnd(false)

#define TEST_BEGIN_WINDOWED TestBegin(true)
#define TEST_END_WINDOWED TestEnd(true)

static void TestBegin(bool gui)
{
	gProjectPath = PlatformDirectories::GetLaunchDir();
	gProjectName = MODULE_NAME;
	
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");
	ModuleManager::Get().LoadModule("CoreInput");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreShader");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("VulkanRHI");
	ModuleManager::Get().LoadModule("CoreRPI");
	ModuleManager::Get().LoadModule("CrystalWidgets");
	ModuleManager::Get().LoadModule("System");
	ModuleManager::Get().LoadModule("GameSystem");

	PlatformApplication* app = PlatformApplication::Get();

	app->Initialize();

	if (gui)
	{
		PlatformWindowInfo windowInfo{};
		windowInfo.fullscreen = windowInfo.hidden = windowInfo.maximised = windowInfo.resizable = false;
		windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;

		app->InitMainWindow("MainWindow", 1024, 768, windowInfo);

		InputManager::Get().Initialize(app);
	}

	gDynamicRHI = new Vulkan::VulkanRHI();

	gDynamicRHI->Initialize();
	gDynamicRHI->PostInitialize();

	RPISystem::Get().Initialize();
}

static void TestEnd(bool gui)
{
	RPISystem::Get().Shutdown();

	gDynamicRHI->PreShutdown();
	gDynamicRHI->Shutdown();
	delete gDynamicRHI; gDynamicRHI = nullptr;

	PlatformApplication* app = PlatformApplication::Get();

	if (gui)
	{
		InputManager::Get().Shutdown(app);
	}

	app->PreShutdown();
	app->Shutdown();
	delete app;

	ModuleManager::Get().UnloadModule("GameSystem");
	ModuleManager::Get().UnloadModule("System");
	ModuleManager::Get().UnloadModule("CrystalWidgets");
	ModuleManager::Get().UnloadModule("CoreRPI");
	ModuleManager::Get().UnloadModule("VulkanRHI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreInput");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");
}


TEST(RPI, Scene)
{
	TEST_BEGIN_WINDOWED;
	CERegisterModuleTypes();

	auto app = PlatformApplication::Get();

	gEngine->PreInit();
	gEngine->Initialize();
	gEngine->PostInitialize();

	RendererSubsystem* rendererSubsystem = gEngine->GetSubsystem<RendererSubsystem>();
	SceneSubsystem* sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

	CE::Scene* scene = sceneSubsystem->GetActiveScene();
	RPI::Scene* rpiScene = scene->GetRpiScene();

	TestFeatureProcessor1* fp1 = rpiScene->AddFeatureProcessor<TestFeatureProcessor1>();
	StaticMeshFeatureProcessor* meshFp = rpiScene->GetFeatureProcessor<StaticMeshFeatureProcessor>();

	CE::Shader* standardShader = AssetManager::Get()->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
	
	{
		StaticMeshActor* meshActor = CreateObject<StaticMeshActor>(scene, "StaticMesh");
		scene->AddActor(meshActor);

		StaticMeshComponent* meshComponent = meshActor->GetMeshComponent();

		StaticMesh* cubeMesh = CreateObject<StaticMesh>(meshComponent, "StaticMesh");
		{
			RPI::ModelAsset* cubeModel = CreateObject<ModelAsset>(cubeMesh, "CubeModel");
			RPI::ModelLodAsset* cubeLodAsset = RPI::ModelLodAsset::CreateCubeAsset(cubeModel);
			cubeModel->AddModelLod(cubeLodAsset);

			cubeMesh->SetModelAsset(cubeModel);
		}
		meshComponent->SetStaticMesh(cubeMesh);

		meshComponent->SetLocalPosition(Vec3(0, 0, 10));

		CE::Material* material = CreateObject<CE::Material>(meshComponent, "Material");
		meshComponent->SetMaterial(material, 0, 0);

		SceneComponent* subComponent = CreateObject<SceneComponent>(meshComponent, "SubComponent");
		meshComponent->SetupAttachment(subComponent);

		CameraComponent* cameraComponent = CreateObject<CameraComponent>(meshComponent, "Camera");
		cameraComponent->SetLocalPosition(Vec3(0, 0, -10));
		meshComponent->SetupAttachment(cameraComponent);

		clock_t lastTime = clock();
		constexpr int LoopCount = 32;

		auto tick = [&]()
			{
				auto curTime = clock();
				f32 deltaTime = ((f32)(curTime - lastTime)) / CLOCKS_PER_SEC;

				app->Tick();
				InputManager::Get().Tick();

				gEngine->Tick(deltaTime);

				lastTime = curTime;
			};

		for (int i = 0; i < LoopCount; i++)
		{
			tick();
		}

		HashSet<ActorComponent*> allComponents{};

		scene->IterateAllComponents<ActorComponent>([&](ActorComponent* actorComponent)
		{
			allComponents.Add(actorComponent);
		});
		EXPECT_EQ(allComponents.GetSize(), 3);

		allComponents.Clear();

		meshComponent->DetachComponent(subComponent);

		scene->IterateAllComponents<ActorComponent>([&](ActorComponent* actorComponent)
			{
				allComponents.Add(actorComponent);
			});
		EXPECT_EQ(allComponents.GetSize(), 2);

		subComponent->Destroy();
	}

	gEngine->PreShutdown();
	gEngine->Shutdown();

	CEDeregisterModuleTypes();
	TEST_END_WINDOWED;
}

