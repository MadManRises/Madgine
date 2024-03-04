#include "../firstpartylib.h"

#include "firstpartyservices.h"

#include "Madgine/root/root.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::FirstParty::FirstPartyServices)
METATABLE_END(Engine::FirstParty::FirstPartyServices)

VIRTUALUNIQUECOMPONENTBASE(Engine::FirstParty::FirstPartyServices)

namespace Engine {
namespace FirstParty {

    FirstPartyServices::FirstPartyServices(Root::Root &root)
        : VirtualRootComponentBase(root)
    {
    }

    Threading::TaskFuture<Leaderboard> FirstPartyServices::getLeaderboard(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd)
    {
        return mRoot.taskQueue()->queueTask(getLeaderboardTask(name, accessmode, referenceRank, rangeBegin, rangeEnd));
    }

    Threading::TaskFuture<bool> FirstPartyServices::ingestStat(const char *name, const char *leaderboardName, int32_t value)
    {
        return mRoot.taskQueue()->queueTask(ingestStatTask(name, leaderboardName, value));
    }

    Threading::TaskFuture<bool> FirstPartyServices::unlockAchievement(const char *name)
    {
        return mRoot.taskQueue()->queueTask(unlockAchievementTask(name));
    }

}
}
