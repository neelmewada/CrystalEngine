
#include "IO/FileWatcher.h"

#include "efsw/efsw.hpp"

namespace CE::IO
{

    class FileWatchListernerImpl : public efsw::FileWatchListener
    {
    public:

        FileWatchListernerImpl(IFileWatchListener* listener) : listener(listener)
        {}

        // Inherited via FileWatchListener
        virtual void handleFileAction(
            efsw::WatchID watchid, 
            const std::string& dir, 
            const std::string& filename,
            efsw::Action action, 
            std::string oldFilename) override
        {
            if (listener != nullptr)
            {
                listener->HandleFileAction((CE::IO::WatchID)watchid, Path(dir), filename, (FileAction)action, oldFilename);
            }
        }

    private:
        IFileWatchListener* listener = nullptr;
    };
    
    FileWatcher::FileWatcher()
        : fileWatcherImpl(new efsw::FileWatcher())
    {
        
    }

    FileWatcher::~FileWatcher()
    {
        if (watchIdToListenerMap.GetSize() == 0)
        {
            delete fileWatcherImpl;
            return;
        }

        for (int i = 0; i < watchIds.GetSize(); i++)
        {
            RemoveWatcher(watchIds[i]);
        }

        delete fileWatcherImpl;
    }

    WatchID FileWatcher::AddWatcher(IO::Path directory, IFileWatchListener* listener, bool recursive)
    {
        String dir = directory.GetString();
        auto watcher = new FileWatchListernerImpl(listener);
        
        WatchID watchId = static_cast<WatchID>(fileWatcherImpl->addWatch(std::string(dir.GetCString()), watcher, recursive));
        watchIdToListenerMap.Add({ watchId, watcher });
        watchIds.Add(watchId);
        return watchId;
    }

    void FileWatcher::RemoveWatcher(WatchID watchId)
    {
        if (!watchIdToListenerMap.KeyExists(watchId))
            return;

        delete watchIdToListenerMap[watchId];
        watchIdToListenerMap.Remove(watchId);
        watchIds.Remove(watchId);

        fileWatcherImpl->removeWatch(static_cast<efsw::WatchID>(watchId));
    }

    void FileWatcher::Watch()
    {
        fileWatcherImpl->watch();
    }

} // namespace CE::IO

