#pragma once

#include "CoreTypes.h"

namespace CE::Editor
{

    /// Holds meta information about a project
    struct ProjectMetaData
    {
    public:
        String projectName{};
        String engineVersion = "0.0.1";

    };
    
} // namespace CE::Editor
