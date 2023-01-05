#pragma once

#include "RHI/RHI.h"

namespace CE
{

    class NUKLEARVULKANRHI_API NuklearVulkanRHI : public DynamicRHI
    {
    public:

        virtual void Initialize() override;
        virtual void PreShutdown() override;
        virtual void Shutdown() override;
        
    };
    
} // namespace CE
