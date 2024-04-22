#include "Core.h"

#include "GameSystem.h"
#include "VulkanRHI.h"

#include <gtest/gtest.h>

using namespace CE;

#include "TestFeatureProcessor.h"

#include "GameSystem_Test.private.h"

using namespace CE;

#define TEST_BEGIN TestBegin()
#define TEST_END TestEnd()

static void TestBegin()
{
	gProjectPath = PlatformDirectories::GetLaunchDir();
	gProjectName = MODULE_NAME;

	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");
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

	gDynamicRHI = new Vulkan::VulkanRHI();

	gDynamicRHI->Initialize();
	gDynamicRHI->PostInitialize();

	RPISystem::Get().Initialize();
}

static void TestEnd()
{
	RPISystem::Get().Shutdown();

	gDynamicRHI->PreShutdown();
	gDynamicRHI->Shutdown();
	delete gDynamicRHI; gDynamicRHI = nullptr;

	PlatformApplication* app = PlatformApplication::Get();
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
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");
}


TEST(RPI, Scene)
{
	TEST_BEGIN;
	CERegisterModuleTypes();

	CE::Scene* scene = CreateObject<CE::Scene>(nullptr, "Scene");
	RPI::Scene* rpiScene = scene->GetRpiScene();

	RendererSubsystem* rendererSubsystem = gEngine->GetSubsystem<RendererSubsystem>();

	TestFeatureProcessor1* fp1 = rpiScene->AddFeatureProcessor<TestFeatureProcessor1>();
	
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

		constexpr f32 delta = 0.016f;

		gEngine->Tick(delta);


	}

	scene->Destroy();

	CEDeregisterModuleTypes();
	TEST_END;
}

