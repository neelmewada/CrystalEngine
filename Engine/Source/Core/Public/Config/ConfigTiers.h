#pragma once

#include "ConfigTypes.h"

namespace CE
{

    static const ConfigTier gConfigTiers[] = {
        { "AbsoluteBase", "{ENGINE}/Config/Base.ini" },
        
        { "Base", "{ENGINE}/Config/Base{TYPE}.ini" },
        { "BasePlatform", "{ENGINE}/Config/{PLATFORM}/Base{PLATFORM}{TYPE}.ini" },

        { "ProjectDefault", "{PROJECT}/Config/Default{TYPE}.ini" },
        { "ProjectGenerated", "{PROJECT}/Config/Generated{TYPE}.ini" },

        { "EnginePlatform", "{ENGINE}/Config/{PLATFORM}/{PLATFORM}{TYPE}.ini" },
        { "ProjectPlatform", "{PROJECT}/Config/{PLATFORM}{TYPE}.ini" },
    };
    
}

