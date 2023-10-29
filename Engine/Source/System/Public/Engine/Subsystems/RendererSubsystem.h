#pragma once

namespace CE
{
	class SceneSubsystem;

	CLASS()
	class SYSTEM_API RendererSubsystem : public EngineSubsystem
	{
		CE_CLASS(RendererSubsystem, EngineSubsystem)
	public:

		RendererSubsystem();

	protected:

		void Initialize() override;

		void PostInitialize() override;

		void Shutdown() override;

		void Tick(f32 delta) override;

	protected:

		// - Fields -

		FIELD()
		SceneSubsystem* sceneSubsystem = nullptr;

		RHI::GraphicsPipelineState* errorPipeline = nullptr;
	};
    
} // namespace CE

#include "RendererSubsystem.rtti.h"
