#include "System.h"

namespace CE
{
    SceneSubsystem::SceneSubsystem()
    {
		
    }

    void SceneSubsystem::RegisterViewport(CWindow* viewport, CE::Scene* scene)
    {
		if (!viewport || !scene)
			return;

		scenesByViewport[viewport] = scene;

		renderer->RebuildFrameGraph();
    }

    void SceneSubsystem::SetMainViewport(CWindow* viewport)
    {
		mainViewport = viewport;

		renderer->RebuildFrameGraph();
    }

    void SceneSubsystem::Initialize()
	{
		Super::Initialize();

		renderer = gEngine->GetSubsystem<RendererSubsystem>();
	}

	void SceneSubsystem::PostInitialize()
	{
		Super::PostInitialize();

		// TODO: Implement multi scene support
		
		// Create & set an empty scene by default
		mainScene = CreateObject<CE::Scene>(this, TEXT("EmptyScene"));
	}

	void SceneSubsystem::PreShutdown()
	{
		mainViewport = nullptr;

		if (mainScene)
		{
			mainScene->Destroy();
			mainScene = nullptr;
		}

		Super::PreShutdown();
	}

	void SceneSubsystem::Shutdown()
	{

		Super::Shutdown();
	}

	void SceneSubsystem::Tick(f32 deltaTime)
	{
		Super::Tick(deltaTime);

		if (!isPlaying)
		{
			isPlaying = true;

			if (mainScene != nullptr)
				mainScene->OnBeginPlay();
		}
		
		if (mainScene != nullptr)
		{
			mainScene->Tick(deltaTime);
		}
	}

	void SceneSubsystem::OnSceneDestroyed(CE::Scene* scene)
	{
		otherScenes.Remove(scene);

		if (scene == mainScene)
		{
			mainScene = nullptr;
		}
	}

} // namespace CE
