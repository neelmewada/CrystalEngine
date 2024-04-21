#pragma once

namespace CE::RPI
{
	class FeatureProcessor;

	class CORERPI_API Scene final
	{
	public:

		Scene();
		~Scene();

		FeatureProcessor* AddFeatureProcessor(SubClass<FeatureProcessor> classType);

		template<typename TFeatureProcessor> requires TIsBaseClassOf<FeatureProcessor, TFeatureProcessor>::Value
		TFeatureProcessor* AddFeatureProcessor()
		{
			return AddFeatureProcessor(TFeatureProcessor::Type());
		}

		void Simulate(f32 currentTime);

		void PrepareRender(f32 currentTime);

		void CollectDrawPackets();

	private:

		Array<RenderPipeline*> renderPipelines{};

		Array<FeatureProcessor*> featureProcessors{};

		FeatureProcessor::SimulatePacket simulatePacket{};
		FeatureProcessor::RenderPacket renderPacket{};

		/// @brief A hash map of all views owned by this scene accessed by their respective tags
		PipelineViewsByTag viewsByTag{};

	};
    
} // namespace CE::RPI
