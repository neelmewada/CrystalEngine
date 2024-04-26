#pragma once

namespace CE::RHI
{
    
    interface IScopeProducer
    {
    protected:
        IScopeProducer() = default;

        virtual ~IScopeProducer() = default;
        
    public:
        
        virtual void ProduceScopes(FrameScheduler* scheduler) = 0;

        virtual void EmplaceAttachments(FrameAttachmentDatabase& attachmentDatabase) = 0;
        
    };
    
} // namespace CE::RHI
