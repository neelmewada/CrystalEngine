#include "System.h"

namespace CE
{

    CE::RenderPipeline::RenderPipeline()
    {
        if (!IsDefaultInstance())
        {
	        renderPipeline = new RPI::RenderPipeline();
        }
    }

    CE::RenderPipeline::~RenderPipeline()
    {
        delete renderPipeline; renderPipeline = nullptr;
    }

    void CE::RenderPipeline::ConstructPipeline()
    {
        
    }

} // namespace CE
