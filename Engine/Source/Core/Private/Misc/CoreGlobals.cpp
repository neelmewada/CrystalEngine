
#include "CoreMinimal.h"

namespace CE
{
    /* *******************************************
     *  Global Variables
     */

    CORE_API IO::Path gProjectPath{};

    CORE_API String gProjectName{};

    CORE_API u32 gDefaultWindowWidth = 1280;
    CORE_API u32 gDefaultWindowHeight = 720;

    CORE_API bool gIsEngineRequestingExit = false;
    CORE_API String gEngineExitMessage{};

    CORE_API Array<String> gProgramArguments{};
    
    CORE_API ConfigCache* gConfigCache = nullptr;

    /* *******************************************
     *  Global Functions
     */
    
    CORE_API void RequestEngineExit(String exitMessage)
    {
        gEngineExitMessage = exitMessage;
        gIsEngineRequestingExit = true;
    }

    CORE_API bool IsEngineRequestingExit()
    {
        return gIsEngineRequestingExit;
    }

}

