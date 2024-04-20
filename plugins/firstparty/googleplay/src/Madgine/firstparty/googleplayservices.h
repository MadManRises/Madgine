#pragma once

#include "Madgine/firstparty/firstpartyservices.h"

#include "Modules/threading/taskqueue.h"

//#include <steam/steam_api.h>

namespace Engine {
namespace FirstParty {

    struct GooglePlayServices : FirstPartyServicesImpl<GooglePlayServices> {

        GooglePlayServices(Root::Root &root);
        ~GooglePlayServices();

        virtual std::string_view key() const override;

        ////////// LEADERBOARD

        virtual Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd) override;

        /////////// STATS        

        virtual Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value) override;
        
        /////////// ACHIEVEMENTS

        virtual Threading::Task<bool> unlockAchievementTask(const char *name) override;

        /////////// MATCHMAKING

        virtual Threading::Task<std::vector<Lobby>> getLobbyListTask() override;
        virtual Threading::Task<std::optional<Lobby>> createLobbyTask(MatchmakingCallback cb) override;
        virtual Threading::Task<std::optional<Lobby>> joinLobbyTask(uint64_t id, MatchmakingCallback cb) override;
        virtual Threading::Task<bool> startMatchTask() override;
        virtual void leaveLobby() override;
        virtual Threading::Task<std::optional<LobbyInfo>> getCurrentLobbyInfoTask() override;

        //////////////////        
    };

}
}

REGISTER_TYPE(Engine::FirstParty::GooglePlayServices)