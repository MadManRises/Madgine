#pragma once

#include "Madgine/firstparty/firstpartyservices.h"

#include "Modules/threading/taskqueue.h"

#include <steam/steam_api.h>

namespace Engine {
namespace FirstParty {

    struct SteamServices : FirstPartyServicesImpl<SteamServices> {

        SteamServices(Root::Root &root);
        ~SteamServices();

        virtual std::string_view key() const override;

        ////////// LEADERBOARD

        virtual Threading::TaskFuture<Leaderboard> getLeaderboard(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd, uint32_t *fullSize) override;
        Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd, uint32_t *fullSize);

        /////////// STATS

        virtual Threading::TaskFuture<bool> ingestStat(const char *name, const char *leaderboardName, int32_t value) override;

        Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value);
        void requestCurrentStats();

        Threading::TaskPromise<bool> mStatsRequestedPromise;
        Threading::TaskFuture<bool> mStatsRequestedFuture;
        STEAM_CALLBACK(SteamServices, onUserStatsReceived, UserStatsReceived_t);

        //////////////////

        bool mInitialized = false;

        Threading::TaskQueue mQueue;
    };

}
}

REGISTER_TYPE(Engine::FirstParty::SteamServices)