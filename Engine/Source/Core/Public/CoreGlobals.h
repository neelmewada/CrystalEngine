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
    
    /// Path to the executable to run when this program shuts down and exits.
    extern CORE_API IO::Path gExitLaunchProcess;

    /// The arguments to pass for gExitLaunchProcess executable.
    extern CORE_API String gExitLaunchArguments;

    extern CORE_API Array<String> gProgramArguments;
    
    extern CORE_API ConfigCache* gConfigCache;

    /* *******************************************
     *  Global Functions
     */

	/// Returns true if we're running inside an editor build. Applies to all editors, tools and programs (AutoRTTI, etc) built in editor mode.
	CORE_API bool IsEditor();

	/// Returns true if we're running inside a runtime build.
	CORE_API bool IsRuntime();

    CORE_API void RequestEngineExit(String exitMessage);

    CORE_API bool IsEngineRequestingExit();
 
}
