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

VIRTUALUNIQUECOMPONENT(Engine::FirstParty::SteamServices)

namespace Engine {
namespace FirstParty {

    SteamServices::SteamServices(Root::Root &root)
        : FirstPartyServicesImpl<SteamServices>(root)
    {

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

    Threading::Task<Leaderboard> SteamServices::getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd, uint32_t *fullSize)
    {
        ELeaderboardDataRequest requestmode;
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

        LeaderboardFindResult_t leaderboard = co_await steam_sender<LeaderboardFindResult_t>(SteamUserStats()->FindLeaderboard(name));
        if (!leaderboard.m_bLeaderboardFound)
            co_return {};

        LeaderboardScoresDownloaded_t download = co_await steam_sender<LeaderboardScoresDownloaded_t>(SteamUserStats()->DownloadLeaderboardEntries(leaderboard.m_hSteamLeaderboard, requestmode, rangeBegin, rangeEnd));

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

        LeaderboardFindResult_t leaderboardFound = co_await steam_sender<LeaderboardFindResult_t>(SteamUserStats()->FindLeaderboard(leaderboardName));
        if (!leaderboardFound.m_bLeaderboardFound)
            co_return false;

        auto upload = steam_sender<LeaderboardScoreUploaded_t>(SteamUserStats()->UploadLeaderboardScore(leaderboardFound.m_hSteamLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, value, nullptr, 0));

        bool success = SteamUserStats()->StoreStats();

        co_return success && (co_await upload).m_bSuccess;
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

}
}
