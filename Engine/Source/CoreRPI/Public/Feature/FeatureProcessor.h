#pragma once

namespace CE::RPI
{
	class View;
	class Scene;

	CLASS(Abstract)
	class CORERPI_API FeatureProcessor : public Object
	{
		CE_CLASS(FeatureProcessor, Object)
	public:

		FeatureProcessor() = default;
		virtual ~FeatureProcessor() = default;

		struct SimulatePacket
		{
			Job* parentJob = nullptr;
		};

		struct RenderPacket
		{
			Array<ViewPtr> views{};

			RHI::DrawListMask drawListMask{};

		};

		virtual void Simulate(const SimulatePacket& packet) {}

		//! @brief The feature processor should enqueue draw packets to relevant draw lists.
		virtual void Render(const RenderPacket& packet) {}

		virtual void OnRenderEnd() {}

	protected:

		RPI::Scene* scene = nullptr;

		friend class Scene;
	};
    
} // namespace CE::RPI

#include "FeatureProcessor.rtti.h"
