#include "../../metalib.h"

#include "syncstreamdata.h"

#include "../syncmanager.h"

namespace Engine {
namespace Serialize {

    SyncStreamData::SyncStreamData(SyncManager &mgr, ParticipantId id)
        : SerializeStreamData(mgr, id)
    {
    }

    SyncManager *SyncStreamData::manager()
    {
        return static_cast<SyncManager *>(SerializeStreamData::manager());
    }

}
}