#pragma once

namespace CE
{
	class Scene;
	class RendererSubsystem;

    CLASS()
	class SYSTEM_API SceneSubsystem : public EngineSubsystem
	{
		CE_CLASS(SceneSubsystem, EngineSubsystem)
	public:
		SceneSubsystem();

		CE::Scene* GetMainScene() { return mainScene; }

		void RegisterViewport(CWindow* viewport, CE::Scene* scene);

		void SetMainViewport(CWindow* viewport);

		CWindow* GetMainViewport() const { return mainViewport; }

	protected:

		void Initialize() override;
		void PostInitialize() override;
		void PreShutdown() override;
		void Shutdown() override;

		void Tick(f32 deltaTime) override;

		void OnSceneDestroyed(CE::Scene* scene);

		FIELD()
		CE::Scene* mainScene = nullptr;
		
		FIELD()
		CWindow* mainViewport = nullptr;

		FIELD()
		Array<CE::Scene*> otherScenes{};

		HashMap<CWindow*, CE::Scene*> scenesByViewport{};

    private:

		RendererSubsystem* renderer = nullptr;

		bool isPlaying = false;

		friend class RendererSubsystem;
		friend class CE::Scene;
	};

} // namespace CE

#include "SceneSubsystem.rtti.h"
