#pragma once

#include "CoreMinimal.h"

namespace CE
{

    struct SYSTEM_API ProjectSettings
    {
        CE_STRUCT(ProjectSettings)

    public:

        String projectName{};
        String projectVersion{};

    };
    
} // namespace CE

CE_RTTI_STRUCT(SYSTEM_API, CE, ProjectSettings,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(projectName)
        CE_FIELD(projectVersion)
    )
)
