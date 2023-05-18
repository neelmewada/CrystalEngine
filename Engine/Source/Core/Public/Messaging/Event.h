#pragma once

#include "Object/Object.h"

namespace CE
{
    
    enum class EventResult
    {
        DontHandle,
        HandleAndPropagate,
        HandleAndStopPropagation
    };

} // namespace CE

CE_RTTI_ENUM(CORE_API, CE, EventResult,
    CE_ATTRIBS(),
    CE_CONST(DontHandle),
    CE_CONST(HandleAndPropagate),
    CE_CONST(HandleAndStopPropagation)
)
