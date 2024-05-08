#pragma once

namespace CE
{
	class Scene;

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

		FIELD()
		CE::Scene* mainScene = nullptr;

    private:

		bool isPlaying = false;
	};

} // namespace CE

#include "SceneSubsystem.rtti.h"
