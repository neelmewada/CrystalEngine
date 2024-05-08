#pragma once

namespace CE::RHI
{
    
    interface IScopeProducer
    {
    protected:
        IScopeProducer() = default;

        virtual ~IScopeProducer() = default;
        
    public:

        virtual const Name& GetPassName() const = 0;
        
        virtual void ProduceScopes(FrameScheduler* scheduler) = 0;

        virtual void EmplaceAttachments(FrameScheduler* scheduler) = 0;
        
    };
    
} // namespace CE::RHI
