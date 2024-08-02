#pragma once

#include <steam/steam_api.h>

#include "Meta/serialize/streams/syncstreamdata.h"

namespace Engine {
namespace FirstParty {

    struct SteamStreamData : Serialize::SyncStreamData {
        SteamStreamData(SteamSyncManager &mgr, Serialize::ParticipantId id, CSteamID user);

        CSteamID user() const;

    private:
        CSteamID mUser;
    };

}
}