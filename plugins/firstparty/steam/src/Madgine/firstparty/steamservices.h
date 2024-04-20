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

        virtual Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd) override;

        /////////// STATS        

        virtual Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value) override;
        void requestCurrentStats();

        Threading::TaskPromise<bool> mStatsRequestedPromise;
        Threading::TaskFuture<bool> mStatsRequestedFuture;
        STEAM_CALLBACK(SteamServices, onUserStatsReceived, UserStatsReceived_t);

        /////////// ACHIEVEMENTS

        virtual Threading::Task<bool> unlockAchievementTask(const char *name) override;

        /////////// MATCHMAKING

        virtual Threading::Task<std::vector<Lobby>> getLobbyListTask() override;
        virtual Threading::Task<std::optional<Lobby>> createLobbyTask(MatchmakingCallback cb) override;
        virtual Threading::Task<std::optional<Lobby>> joinLobbyTask(uint64_t id, MatchmakingCallback cb) override;
        virtual Threading::Task<bool> startMatchTask() override;
        virtual void leaveLobby() override;
        virtual void setLobbyInfoCallback(LobbyInfoCallback cb) override;

        void updateLobbyInfo();

        CSteamID mCurrentLobby;
        MatchmakingCallback mCurrentMatchmakingCallback;
        LobbyInfoCallback mLobbyInfoCallback;

        STEAM_CALLBACK(SteamServices, onLobbyInfoUpdate, LobbyDataUpdate_t);

        //////////////////

        bool mInitialized = false;
    };

}
}

REGISTER_TYPE(Engine::FirstParty::SteamServices)