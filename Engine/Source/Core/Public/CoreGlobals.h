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
    
    extern CORE_API ConfigCache* gConfigCache;
    
}
