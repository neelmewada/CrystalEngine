#include "GameSystem.h"

namespace CE
{

	GameViewportSubsystem::GameViewportSubsystem()
	{
		
	}

	GameViewportSubsystem::~GameViewportSubsystem()
	{

	}

	void GameViewportSubsystem::Initialize()
	{
		Super::Initialize();
	}

	void GameViewportSubsystem::PostInitialize()
	{
		Super::PostInitialize();

		CApplication* app = CApplication::TryGet();
		auto mainWindow = PlatformApplication::Get()->GetMainWindow();

		SceneSubsystem* sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();
		CE::Scene* activeScene = sceneSubsystem->GetMainScene();

		if (mainWindow && app)
		{
			gameWindow = CreateWindow<CGameWindow>(gProjectName, mainWindow);

			sceneSubsystem->SetMainViewport(gameWindow);
		}
	}

	void GameViewportSubsystem::PreShutdown()
	{
		Super::PreShutdown();
	}

	void GameViewportSubsystem::Shutdown()
	{
		Super::Shutdown();
	}

	f32 GameViewportSubsystem::GetTickPriority() const
	{
		return 3;
	}

} // namespace CE
