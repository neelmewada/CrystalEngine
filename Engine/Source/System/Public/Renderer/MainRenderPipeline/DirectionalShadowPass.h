#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API DirectionalShadowPass : public RPI::RasterPass
    {
        CE_CLASS(DirectionalShadowPass, RPI::RasterPass)
    public:

        DirectionalShadowPass();
        ~DirectionalShadowPass();
        
    private:

        void ProduceScopes(FrameScheduler* scheduler) override;
        
        void EmplaceAttachments(FrameAttachmentDatabase& attachmentDatabase) override;

        
    };
    
} // namespace CE

#include "DirectionalShadowPass.rtti.h"