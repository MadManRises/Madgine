#include "../interfaceslib.h"

#if ANDROID

#    include "filewatcher.h"

namespace Engine {
namespace Filesystem {

    FileWatcher::FileWatcher()
    {
    }

    FileWatcher::~FileWatcher()
    {
    }

    void FileWatcher::addWatch(const Path &path)
    {

    }

    void FileWatcher::removeWatch(const Path &path)
    {

    }

    std::vector<FileEvent> FileWatcher::fetchChanges()
    {
        return {};
    }

}
}

#endif