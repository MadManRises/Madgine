#pragma once

#include "Meta/serialize/syncmanager.h"


namespace Engine {
namespace FirstParty {

    struct FirstPartySyncManager : Serialize::SyncManager {
        using Serialize::SyncManager::SyncManager;
    };

}
}