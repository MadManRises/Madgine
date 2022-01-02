#include "../interfaceslib.h"

#include "filewatcher.h"

namespace Engine {
namespace Filesystem {

    std::vector<FileEvent> FileWatcher::fetchChangesReduced()
    {
        std::vector<FileEvent> events = fetchChanges();
        for (auto it = events.rbegin(); it != events.rend(); ++it) {
            switch (it->mType) {
            case Filesystem::FileEventType::FILE_DELETED:
                for (auto it2 = std::next(it); it2 != events.rend(); ++it2) {
                    bool abort = false;
                    if (it2->mPath == it->mPath) {
                        switch (it2->mType) {
                        case Filesystem::FileEventType::FILE_CREATED:
                            abort = true;
                            it2->mType = Filesystem::FileEventType::MIN;
                            it->mType = Filesystem::FileEventType::MIN;
                            break;
                        case Filesystem::FileEventType::FILE_MODIFIED:
                            it2->mType = Filesystem::FileEventType::MIN;
                            break;
                        case Filesystem::FileEventType::FILE_RENAMED:
                            it2->mType = Filesystem::FileEventType::FILE_DELETED;
                            it->mType = Filesystem::FileEventType::MIN;
                            it2->mPath = it2->mOldPath;
                            abort = true;
                            break;
                        }
                    }
                    if (abort)
                        break;
                }
                break;
            case Filesystem::FileEventType::FILE_RENAMED:
                for (auto it2 = std::next(it); it2 != events.rend(); ++it2) {
                    bool abort = false;
                    if (it2->mPath == it->mPath) {
                        switch (it2->mType) {
                        case Filesystem::FileEventType::FILE_DELETED:
                            abort = true;
                            it2->mType = Filesystem::FileEventType::FILE_DELETED;
                            it2->mPath = it->mOldPath;
                            it->mType = Filesystem::FileEventType::FILE_MODIFIED;
                            break;
                        }
                    }
                    if (abort)
                        break;
                }
                break;
            }
        }

        std::erase_if(events, [](const Filesystem::FileEvent &event) { return event.mType == Filesystem::FileEventType::MIN; });

        return events;
    }

}
}