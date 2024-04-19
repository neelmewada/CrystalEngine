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
	};
    
} // namespace CE::RPI
