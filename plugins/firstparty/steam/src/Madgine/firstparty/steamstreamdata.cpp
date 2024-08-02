#include "../steamserviceslib.h"

#include "steamstreambuf.h"
#include "steamstreamdata.h"

#include "steamsyncmanager.h"

namespace Engine {
namespace FirstParty {

    SteamStreamData::SteamStreamData(SteamSyncManager &mgr, Serialize::ParticipantId id, CSteamID user)
        : SyncStreamData(mgr, id)
        , mUser(user)
    {
    }

    CSteamID SteamStreamData::user() const
    {
        return mUser;
    }

}
}