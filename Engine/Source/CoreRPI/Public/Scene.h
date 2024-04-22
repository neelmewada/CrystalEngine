#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RPI_Scene_Test;
#endif

namespace CE::RPI
{
	class FeatureProcessor;

	class CORERPI_API Scene final
	{
	public:

		Scene();
		~Scene();

		FeatureProcessor* AddFeatureProcessor(SubClass<FeatureProcessor> classType);

		FeatureProcessor* GetFeatureProcessor(SubClass<FeatureProcessor> classType);

		template<typename TFeatureProcessor> requires TIsBaseClassOf<FeatureProcessor, TFeatureProcessor>::Value
		TFeatureProcessor* AddFeatureProcessor()
		{
			return (TFeatureProcessor*)AddFeatureProcessor(TFeatureProcessor::Type());
		}

		template<typename TFeatureProcessor> requires TIsBaseClassOf<FeatureProcessor, TFeatureProcessor>::Value
		TFeatureProcessor* GetFeatureProcessor()
		{
			return (TFeatureProcessor*)GetFeatureProcessor(TFeatureProcessor::Type());
		}

		void Simulate(f32 currentTime);

		void PrepareRender(f32 currentTime);

		void OnRenderEnd();

		void CollectDrawPackets();

	private:

		Array<RenderPipeline*> renderPipelines{};

		Array<FeatureProcessor*> featureProcessors{};

		FeatureProcessor::SimulatePacket simulatePacket{};
		FeatureProcessor::RenderPacket renderPacket{};

		/// @brief A hash map of all views owned by this scene accessed by their respective tags
		PipelineViewsByTag viewsByTag{};

		friend class ::RPI_Scene_Test;
	};
    
} // namespace CE::RPI
