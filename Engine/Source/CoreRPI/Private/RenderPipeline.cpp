#include "CoreRPI.h"

namespace CE::RPI
{

    RenderPipeline::~RenderPipeline()
    {

    }

	RenderPipeline* RenderPipeline::Create(RHI::RenderTarget* renderTarget, const RenderPipelineDesc& desc)
	{
		RenderPipeline* renderPipeline = new RenderPipeline();
		renderPipeline->renderTarget = renderTarget;

		renderPipeline->Initialize(desc);

		return renderPipeline;
	}

    void RenderPipeline::Initialize(const RenderPipelineDesc& desc)
    {
		this->desc = desc;

		name = desc.name;
		rootPassTemplateName = desc.rootPassTemplate;
		mainViewTag = desc.mainViewTag;

    }

} // namespace CE::RPI
