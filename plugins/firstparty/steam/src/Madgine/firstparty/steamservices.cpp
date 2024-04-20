#include "../steamserviceslib.h"

#include "steamservices.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

#include <steam/steam_api.h>

#include "steam_sender.h"

#include "Modules/threading/awaitables/awaitabletimepoint.h"

#include "Madgine/root/root.h"

#include "Modules/threading/awaitables/awaitablesender.h"

METATABLE_BEGIN_BASE(Engine::FirstParty::SteamServices, Engine::FirstParty::FirstPartyServices)
READONLY_PROPERTY(Initialized, mInitialized)
METATABLE_END(Engine::FirstParty::SteamServices)

UNIQUECOMPONENT(Engine::FirstParty::SteamServices)

namespace Engine {
namespace FirstParty {

    SteamServices::SteamServices(Root::Root &root)
        : FirstPartyServicesImpl<SteamServices>(root)
    {
        if (SteamAPI_RestartAppIfNecessary(STEAM_APP_ID)) {
            mErrorCode = -1;
            return;
        }

        mInitialized = SteamAPI_Init();

        root.taskQueue()->queue([this]() -> Threading::Task<void> {
            while (mRoot.taskQueue()->running()) {
                SteamAPI_RunCallbacks();
                co_await 100ms;
            }
        });

        requestCurrentStats();
    }

    SteamServices::~SteamServices()
    {
        SteamAPI_Shutdown();
    }

    std::string_view SteamServices::key() const
    {
        return "SteamServices";
    }

    Threading::Task<Leaderboard> SteamServices::getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd)
    {
        ELeaderboardDataRequest requestmode;
        switch (accessmode) {
        case Leaderboard::GLOBAL:
            switch (referenceRank) {
            case Leaderboard::FIRST:
                requestmode = k_ELeaderboardDataRequestGlobal;
                ++rangeBegin;
                ++rangeEnd;
                break;
            case Leaderboard::USER:
                requestmode = k_ELeaderboardDataRequestGlobalAroundUser;
                break;
            default:
                co_return {};
            }
            break;
        case Leaderboard::FRIENDS:
            requestmode = k_ELeaderboardDataRequestFriends;
            break;
        default:
            co_return {};
        }

        LeaderboardFindResult_t leaderboard = (co_await steam_sender<LeaderboardFindResult_t>(SteamUserStats()->FindLeaderboard(name))).value();
        if (!leaderboard.m_bLeaderboardFound)
            co_return {};

        LeaderboardScoresDownloaded_t download = (co_await steam_sender<LeaderboardScoresDownloaded_t>(SteamUserStats()->DownloadLeaderboardEntries(leaderboard.m_hSteamLeaderboard, requestmode, rangeBegin, rangeEnd))).value();

        Leaderboard result;
        for (size_t i = 0; i < download.m_cEntryCount; ++i) {
            Leaderboard::Entry &entry = result.mEntries.emplace_back();

            LeaderboardEntry_t leaderboardEntry;

            if (!SteamUserStats()->GetDownloadedLeaderboardEntry(download.m_hSteamLeaderboardEntries, i, &leaderboardEntry, nullptr, 0))
                co_return {};

            entry.mRank = leaderboardEntry.m_nGlobalRank;
            entry.mScore = leaderboardEntry.m_nScore;
            entry.mDisplayName = SteamFriends()->GetFriendPersonaName(leaderboardEntry.m_steamIDUser);
        }

        co_return result;
    }

    Threading::Task<bool> SteamServices::ingestStatTask(const char *name, const char *leaderboardName, int32_t value)
    {
        if (!co_await mStatsRequestedFuture)
            co_return false;

        if (!SteamUserStats()->SetStat(name, value))
            co_return false;

        LeaderboardFindResult_t leaderboardFound = (co_await steam_sender<LeaderboardFindResult_t>(SteamUserStats()->FindLeaderboard(leaderboardName))).value();
        if (!leaderboardFound.m_bLeaderboardFound)
            co_return false;

        auto upload = steam_sender<LeaderboardScoreUploaded_t>(SteamUserStats()->UploadLeaderboardScore(leaderboardFound.m_hSteamLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, value, nullptr, 0));

        bool success = SteamUserStats()->StoreStats();

        LeaderboardScoreUploaded_t payload = (co_await std::move(upload)).value();

        co_return success &&payload.m_bSuccess;
    }

    void SteamServices::requestCurrentStats()
    {
        mStatsRequestedFuture = mStatsRequestedPromise.get_future();
        if (!SteamUserStats()->RequestCurrentStats())
            mStatsRequestedPromise.set_value(false);
    }

    void SteamServices::onUserStatsReceived(UserStatsReceived_t *info)
    {
        mStatsRequestedPromise.set_value(info->m_eResult == EResult::k_EResultOK);
    }

    Threading::Task<bool> SteamServices::unlockAchievementTask(const char *name)
    {
        if (!co_await mStatsRequestedFuture)
            co_return false;

        if (!SteamUserStats()->SetAchievement(name))
            co_return false;

        co_return SteamUserStats()->StoreStats();
    }

    Threading::Task<std::vector<Lobby>> SteamServices::getLobbyListTask()
    {
        LobbyMatchList_t result = (co_await steam_sender<LobbyMatchList_t>(SteamMatchmaking()->RequestLobbyList())).value();

        std::vector<Lobby> lobbies;
        lobbies.reserve(result.m_nLobbiesMatching);

        for (size_t i = 0; i < result.m_nLobbiesMatching; ++i) {
            Lobby &lobby = lobbies.emplace_back();
            lobby.mId = SteamMatchmaking()->GetLobbyByIndex(i).ConvertToUint64();
        }

        co_return std::move(lobbies);
    }

    Threading::Task<std::optional<Lobby>> SteamServices::createLobbyTask(MatchmakingCallback cb)
    {
        assert(!mCurrentLobby.IsValid());

        LobbyCreated_t result = (co_await steam_sender<LobbyCreated_t>(SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 2))).value();

        if (result.m_eResult != k_EResultOK) {
            LOG_ERROR("Error creating Steam Lobby: " << result.m_eResult);
            co_return {};
        }

        mCurrentMatchmakingCallback = std::move(cb);
        mCurrentLobby.SetFromUint64(result.m_ulSteamIDLobby);

        updateLobbyInfo();

        co_return Lobby { result.m_ulSteamIDLobby };
    }

    Threading::Task<std::optional<Lobby>> SteamServices::joinLobbyTask(uint64_t id, MatchmakingCallback cb)
    {
        assert(!mCurrentLobby.IsValid());

        LobbyEnter_t result = (co_await steam_sender<LobbyEnter_t>(SteamMatchmaking()->JoinLobby(id))).value();

        if (result.m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess) {
            LOG_ERROR("Error joining Steam Lobby(" << id << "): " << result.m_EChatRoomEnterResponse);
            co_return {};
        }

        mCurrentMatchmakingCallback = std::move(cb);
        mCurrentLobby.SetFromUint64(result.m_ulSteamIDLobby);

        updateLobbyInfo();

        co_return Lobby { result.m_ulSteamIDLobby };
    }

    Threading::Task<bool> SteamServices::startMatchTask()
    {
        assert(mCurrentLobby.IsValid());
    }

    void SteamServices::leaveLobby()
    {
        if (mCurrentLobby.IsValid()) {
            SteamMatchmaking()->LeaveLobby(mCurrentLobby);
            mCurrentLobby.Clear();
            mCurrentMatchmakingCallback = {};
        }
    }

    void SteamServices::setLobbyInfoCallback(LobbyInfoCallback cb)
    {
        mLobbyInfoCallback = std::move(cb);

        updateLobbyInfo();
    }

    void SteamServices::updateLobbyInfo()
    {
        if (mCurrentLobby.IsValid() && mLobbyInfoCallback) {

            int players = SteamMatchmaking()->GetNumLobbyMembers(mCurrentLobby);

            LobbyInfo lobbyInfo;
            for (int i = 0; i < players; ++i) {
                CSteamID user = SteamMatchmaking()->GetLobbyMemberByIndex(mCurrentLobby, i);
                PlayerInfo &info = lobbyInfo.mPlayers.emplace_back();
                info.mName = SteamFriends()->GetFriendPersonaName(user);
            }
            mLobbyInfoCallback(lobbyInfo);
        }
    }

    void SteamServices::onLobbyInfoUpdate(LobbyDataUpdate_t *data)
    {
        updateLobbyInfo();
    }

}
}
