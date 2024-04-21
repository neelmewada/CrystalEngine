#pragma once

namespace CE::RPI
{

	CLASS(Abstract)
	class CORERPI_API LightFeatureProcessor : public FeatureProcessor
	{
		CE_CLASS(LightFeatureProcessor, FeatureProcessor)
	public:

		LightFeatureProcessor();

		virtual ~LightFeatureProcessor();

		void Render(const RenderPacket& packet) override;

	protected:

	};

} // namespace CE::RPI

#include "LightFeatureProcessor.rtti.h"