#include "CoreRHI.h"

namespace CE::RHI
{

    Scope::Scope(const ScopeDescriptor& desc)
    {
		id = desc.id;
		queueClass = desc.queueClass;
    }

    Scope::~Scope()
    {
        
    }
    
} // namespace CE::RHI
