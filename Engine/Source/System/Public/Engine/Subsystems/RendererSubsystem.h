#pragma once

namespace CE
{
	class SceneSubsystem;

	CLASS()
	class SYSTEM_API RendererSubsystem : public EngineSubsystem, ApplicationMessageHandler, CWidgetResourceLoader
	{
		CE_CLASS(RendererSubsystem, EngineSubsystem)
	public:

		RendererSubsystem();

		FrameScheduler* GetScheduler() const { return scheduler; }

	protected:

		RPI::Texture* LoadImage(const Name& assetPath) override;

		void OnWindowCreated(PlatformWindow* window) override;
		void OnWindowDestroyed(PlatformWindow* window) override;
		void OnWindowClosed(PlatformWindow* window) override;
		void OnWindowMinimized(PlatformWindow* window) override;
		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;
		void OnWindowRestored(PlatformWindow* window) override;

		void Initialize() override;

		void PostInitialize() override;

		void PreShutdown() override;

		void Shutdown() override;

		void Tick(f32 delta) override;

		void Render() override;

		void BuildFrameGraph();
		void CompileFrameGraph();

		void SubmitDrawPackets(int imageIndex);

	protected:

		// - Fields -

		FIELD()
		SceneSubsystem* sceneSubsystem = nullptr;

		RHI::FrameScheduler* scheduler = nullptr;

		RHI::DrawListContext drawList{};

		bool rebuildFrameGraph = true;
		bool recompileFrameGraph = true;
	};
    
} // namespace CE

#include "RendererSubsystem.rtti.h"
