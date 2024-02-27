#include "../googleplayserviceslib.h"

#include "googleplayservices.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

//#include <steam/steam_api.h>

//#include "steam_sender.h"

#include "Modules/threading/awaitables/awaitabletimepoint.h"

#include "Madgine/root/root.h"

#include "Modules/threading/awaitables/awaitablesender.h"

#include "playservices/playservices.h"

#include "Generic/execution/execution.h"

METATABLE_BEGIN_BASE(Engine::FirstParty::GooglePlayServices, Engine::FirstParty::FirstPartyServices)
READONLY_PROPERTY(Initialized, mInitialized)
METATABLE_END(Engine::FirstParty::GooglePlayServices)

UNIQUECOMPONENT(Engine::FirstParty::GooglePlayServices)

namespace Engine {
namespace FirstParty {

    GooglePlayServices::GooglePlayServices(Root::Root &root)
        : FirstPartyServicesImpl<GooglePlayServices>(root)
    {
        PlayServices::setup();        
    }

    GooglePlayServices::~GooglePlayServices()
    {
    }

    std::string_view GooglePlayServices::key() const
    {
        return "GooglePlayServices";
    }

    Threading::Task<Leaderboard> GooglePlayServices::getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd, uint32_t *fullSize)
    {
        co_return {};        
    }

    Threading::Task<bool> GooglePlayServices::ingestStatTask(const char *name, const char *leaderboardName, int32_t value)
    {
        co_return false;
    }

    Threading::Task<bool> GooglePlayServices::unlockAchievementTask(const char *name)
    {
        std::optional<std::string> result = co_await PlayServices::Achievements::unlock(name);
        if (result) {
            LOG_ERROR("Error unlocking Google Play Achievement: \n" << *result);
            co_return false;
        } 
        co_return true;
    }

}
}
