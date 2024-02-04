#pragma once

namespace CE::RHI
{

    class CORERHI_API Fence : public RHIResource
    {
    protected:
        Fence() : RHIResource(RHI::ResourceType::Fence)
        {}

    public:

        virtual ~Fence() {}

        virtual void Reset() = 0;

        virtual void WaitForFence() = 0;

        virtual bool IsSignalled() = 0;

    protected:


    };
    
} // namespace CE::RHI
