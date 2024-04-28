#pragma once

#include "Madgine/root/rootcomponentcollector.h"

#include "Madgine/root/rootcomponentbase.h"

#include "Modules/threading/taskfuture.h"

#include "Generic/closure.h"

namespace Engine {
namespace FirstParty {

    struct Leaderboard {

        enum ReferenceRank {
            USER,
            FIRST
        };

        enum AccessMode {
            FRIENDS,
            GLOBAL
        };
        
        struct Entry {
            uintptr_t mUserId;
            std::string mDisplayName;
            size_t mRank;
            int32_t mScore;
        };

        std::vector<Entry> mEntries;
    };

    struct Lobby {
        uint64_t mId;
    };

    struct PlayerInfo {
        std::string mName;
    };

    struct LobbyInfo {
        std::vector<PlayerInfo> mPlayers;
    };

    struct MADGINE_FIRST_PARTY_EXPORT FirstPartyServices : Root::VirtualRootComponentBase<FirstPartyServices> {

        FirstPartyServices(Root::Root &root);

        ///////// LEADERBOARD

        Threading::TaskFuture<Leaderboard> getLeaderboard(const char *name, Leaderboard::AccessMode accessmode = Leaderboard::GLOBAL, Leaderboard::ReferenceRank referenceRank = Leaderboard::FIRST, int32_t rangeBegin = 0, int32_t rangeEnd = 99);
        virtual Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode = Leaderboard::GLOBAL, Leaderboard::ReferenceRank referenceRank = Leaderboard::FIRST, int32_t rangeBegin = 0, int32_t rangeEnd = 99) = 0;

        ///////// STATS

        Threading::TaskFuture<bool> ingestStat(const char *name, const char *leaderboardName, int32_t value);
        virtual Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value) = 0;

        ///////// ACHIEVEMENTS

        Threading::TaskFuture<bool> unlockAchievement(const char *name);
        virtual Threading::Task<bool> unlockAchievementTask(const char *name) = 0;

        ///////// MATCHMAKING

        using MatchmakingCallback = Closure<Serialize::Format(FirstPartySyncManager&)>;
        using SessionStartedCallback = Closure<void()>;
        using LobbyInfoCallback = Closure<void(const LobbyInfo &)>;

        Threading::TaskFuture<std::vector<Lobby>> getLobbyList();
        virtual Threading::Task<std::vector<Lobby>> getLobbyListTask() = 0;

        Threading::TaskFuture<std::optional<Lobby>> createLobby(MatchmakingCallback cb, SessionStartedCallback sessionCb = {});
        virtual Threading::Task<std::optional<Lobby>> createLobbyTask(MatchmakingCallback cb, SessionStartedCallback sessionCb = {}) = 0;

        Threading::TaskFuture<std::optional<Lobby>> joinLobby(uint64_t id, MatchmakingCallback cb, SessionStartedCallback sessionCb);
        virtual Threading::Task<std::optional<Lobby>> joinLobbyTask(uint64_t id, MatchmakingCallback cb, SessionStartedCallback sessionCb) = 0;

        Threading::TaskFuture<bool> startMatch();
        virtual Threading::Task<bool> startMatchTask() = 0;

        virtual void setLobbyInfoCallback(LobbyInfoCallback cb) = 0;        

        virtual bool isLobbyOwner() const = 0;
        virtual void leaveLobby() = 0;
        virtual void leaveMatch() = 0;
    };

    template <typename T>
    using FirstPartyServicesImpl = VirtualScope<T, Root::RootComponentVirtualImpl<T, FirstPartyServices>>;

}
}

REGISTER_TYPE(Engine::FirstParty::FirstPartyServices)