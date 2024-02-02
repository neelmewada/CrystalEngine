#pragma once

namespace CE::RPI
{
	CLASS(Abstract)
	class CORERPI_API FeatureProcessor : public Object
	{
		CE_CLASS(FeatureProcessor, Object)
	public:

		FeatureProcessor() = default;
		virtual ~FeatureProcessor() = default;


	private:


		friend class Scene;
	};
    
} // namespace CE::RPI

#include "FeatureProcessor.rtti.h"
