#pragma once

namespace CE::RPI
{
    CLASS()
	class CORERPI_API RasterPass : public GpuPass
	{
		CE_CLASS(RasterPass, GpuPass)
	public:

		RasterPass();

		virtual ~RasterPass();

	private:

    	void ProduceScopes(FrameScheduler* scheduler) override;

    	void EmplaceAttachments(FrameScheduler* scheduler) override;

	};

} // namespace CE::RPI

#include "RasterPass.rtti.h"
