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

		void Render() override;

	protected:

		// - Fields -

		FIELD()
		SceneSubsystem* sceneSubsystem = nullptr;

		// Temporary stuff

		RHI::GraphicsPipelineState* errorPipeline = nullptr;

		RHI::ShaderResourceGroup* srg0 = nullptr;
		RHI::ShaderResourceGroup* srgEmpty = nullptr;
		RHI::ShaderResourceGroup* srg1 = nullptr;

	};
    
} // namespace CE

#include "RendererSubsystem.rtti.h"
