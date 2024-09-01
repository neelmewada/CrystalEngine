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
		Array<CE::Scene*> otherScenes{};

    private:

		RendererSubsystem* renderer = nullptr;

		bool isPlaying = false;

		friend class RendererSubsystem;
		friend class CE::Scene;
	};

} // namespace CE

#include "SceneSubsystem.rtti.h"
