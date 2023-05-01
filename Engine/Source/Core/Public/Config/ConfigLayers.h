#pragma once

#include "ConfigTypes.h"

namespace CE
{

    static const ConfigLayer gConfigLayers[] = {
        { TEXT("AbsoluteBase"), TEXT("{ENGINE}/Config/Base.ini") },
        
        { TEXT("Base"), TEXT("{ENGINE}/Config/Base{TYPE}.ini") },
        { TEXT("BasePlatform"), TEXT("{ENGINE}/Config/{PLATFORM}/Base{PLATFORM}{TYPE}.ini") },

        { TEXT("ProjectDefault"), TEXT("{PROJECT}/Config/Default{TYPE}.ini") },
        { TEXT("ProjectGenerated"), TEXT("{PROJECT}/Config/Generated{TYPE}.ini") },

        { TEXT("EnginePlatform"), TEXT("{ENGINE}/Config/{PLATFORM}/{PLATFORM}{TYPE}.ini") },
        { TEXT("ProjectPlatform"), TEXT("{PROJECT}/Config/{PLATFORM}{TYPE}.ini") },
    };
    
}

