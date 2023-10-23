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
		

		inline Scene* GetActiveScene() { return scene; }

	protected:

		void Initialize() override;
		void Shutdown() override;

		void Tick(f32 deltaTime) override;

		FIELD()
		Scene* scene = nullptr;
	};

} // namespace CE

#include "SceneSubsystem.rtti.h"
