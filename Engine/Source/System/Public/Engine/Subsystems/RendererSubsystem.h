#pragma once

namespace CE
{
	class SceneSubsystem;
	class ActorComponent;

	CLASS()
	class SYSTEM_API RendererSubsystem : public EngineSubsystem, ApplicationMessageHandler, CWidgetResourceLoader
	{
		CE_CLASS(RendererSubsystem, EngineSubsystem)
	public:

		RendererSubsystem();

		FrameScheduler* GetScheduler() const { return scheduler; }

		FUNCTION()
		void RebuildFrameGraph();

		f32 GetTickPriority() const override;

		void RegisterFeatureProcessor(SubClass<ActorComponent> componentClass, SubClass<FeatureProcessor> fpClass);

		template<typename TActorComponent, typename TFeatureProcessor>
			requires TIsBaseClassOf<ActorComponent, TActorComponent>::Value and TIsBaseClassOf<FeatureProcessor, TFeatureProcessor>::Value
		void RegisterFeatureProcessor()
		{
			RegisterFeatureProcessor(TActorComponent::StaticType(), TFeatureProcessor::StaticType());
		}

		SubClass<FeatureProcessor> GetFeatureProcessClass(SubClass<ActorComponent> componentClass);

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

		void BuildFrameGraph();
		void CompileFrameGraph();

		void SubmitDrawPackets(int imageIndex);

	private:

		// - Fields -
		
		FIELD()
		SceneSubsystem* sceneSubsystem = nullptr;

		HashMap<ClassType*, SubClass<FeatureProcessor>> componentClassToFeatureProcessorClass{};
		
		u32 curImageIndex = 0;

		//CWindow* gameWindow = nullptr;

		//! @brief The viewport used to render the main scene to
		CWindow* primaryViewport = nullptr;

		// - Frame Graph -

		RHI::FrameScheduler* scheduler = nullptr;

		RHI::DrawListContext drawList{};

		bool rebuildFrameGraph = true;
		bool recompileFrameGraph = true;
	};
    
} // namespace CE

#include "RendererSubsystem.rtti.h"
