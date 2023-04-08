#pragma once


namespace efsw
{
    class FileWatcher;
}

namespace CE::IO
{
    typedef long WatchID;

    class FileWatchListernerImpl;

    enum class FileAction
    {
        /// Sent when a file is created or renamed
        Add = 1,
        /// Sent when a file is deleted or renamed
        Delete = 2,
        /// Sent when a file is modified
        Modified = 3,
        /// Sent when a file is moved
        Moved = 4
    };

    class CORE_API IFileWatchListener
    {
    public:
        virtual void HandleFileAction(WatchID watchId, IO::Path directory, String fileName, FileAction fileAction, String oldFileName) = 0;
    };

    class CORE_API FileWatcher final
    {
    public:

        FileWatcher();
        ~FileWatcher();

        WatchID AddWatcher(IO::Path directory, IFileWatchListener* listener, bool recursive = false);

        void RemoveWatcher(WatchID watchId);

        void Watch();

    private:

        efsw::FileWatcher* fileWatcherImpl = nullptr;
        CE::Array<WatchID> watchIds{};
        HashMap<WatchID, FileWatchListernerImpl*> watchIdToListenerMap{};
    };
    
} // namespace CE::IO

CE_RTTI_ENUM(CORE_API, CE::IO, FileAction,
    CE_ATTRIBS(),
    CE_CONST(Add),
    CE_CONST(Delete),
    CE_CONST(Modified),
    CE_CONST(Moved)
)
