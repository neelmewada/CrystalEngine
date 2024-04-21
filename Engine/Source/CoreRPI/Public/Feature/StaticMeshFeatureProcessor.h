#pragma once

namespace CE::RPI
{
	CLASS()
	class CORERPI_API StaticMeshFeatureProcessor : public FeatureProcessor
	{
		CE_CLASS(StaticMeshFeatureProcessor, FeatureProcessor)
	public:

		StaticMeshFeatureProcessor();

		virtual ~StaticMeshFeatureProcessor();

	private:

	};

} // namespace CE::RPI

#include "StaticMeshFeatureProcessor.rtti.h"