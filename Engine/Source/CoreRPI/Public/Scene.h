#pragma once

namespace CE::RPI
{

	class CORERPI_API Scene final
	{
	public:

		Scene();
		~Scene();

	private:

		Array<RenderPipeline*> renderPipelines{};

		Array<FeatureProcessor*> featureProcessors{};

		/// @brief A hash map of all views owned by this scene accessed by their respective tags
		PipelineViewsByTag pipelineViewsByTag{};

	};
    
} // namespace CE::RPI
