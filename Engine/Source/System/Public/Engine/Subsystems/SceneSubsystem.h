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

		CE::Scene* GetActiveScene() { return scene; }

	protected:

		void Initialize() override;
		void PostInitialize() override;
		void PreShutdown() override;
		void Shutdown() override;

		void Tick(f32 deltaTime) override;

		FIELD()
		CE::Scene* scene = nullptr;

    private:

		bool isPlaying = false;
	};

} // namespace CE

#include "SceneSubsystem.rtti.h"
