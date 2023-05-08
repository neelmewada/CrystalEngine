#pragma  once

namespace CE
{
    /*
     *  Forward Declarations
     */
    class ConfigCache;
    namespace IO { class Path; }

    /* *******************************************
     *  Global Variables
     */

    /**
     * \brief In editor, it is the absolute path to project.
     * At runtime, it is the path to root installation directory.
     */
    extern CORE_API IO::Path gProjectPath;

    extern CORE_API String gProjectName;

    extern CORE_API u32 gDefaultWindowWidth;
    extern CORE_API u32 gDefaultWindowHeight;

    extern CORE_API bool gIsEngineRequestingExit;
    extern CORE_API String gEngineExitMessage;

    extern CORE_API Array<String> gProgramArguments;
    
    extern CORE_API ConfigCache* gConfigCache;

    /* *******************************************
     *  Global Functions
     */

    CORE_API void RequestEngineExit(String exitMessage);

    CORE_API bool IsEngineRequestingExit();
 
}
