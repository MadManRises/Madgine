#pragma once

#include "Madgine/firstparty/firstpartyservices.h"

#include "Modules/threading/taskqueue.h"

#include <eos_auth_types.h>
#include <eos_connect_types.h>
#include <eos_leaderboards_types.h>
#include <eos_stats_types.h>
#include <eos_types.h>

namespace Engine {
namespace FirstParty {

    struct EpicServices : FirstPartyServicesImpl<EpicServices> {

        EpicServices(Root::Root &root);
        ~EpicServices();

        Threading::Task<EOS_EpicAccountId> loginUser();
        Threading::Task<EOS_ProductUserId> connectLogin(EOS_EpicAccountId id);

        virtual std::string_view key() const override;

        ////////// LEADERBOARD

        struct LeaderboardInfo {
            std::string mId;
            std::string mStatName;
            enum {
                MIN,
                MAX,
                SUM,
                LATEST
            } mAggregation;
        };

        virtual Threading::TaskFuture<Leaderboard> getLeaderboard(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd, uint32_t *fullSize) override;

        Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode, Leaderboard::ReferenceRank referenceRank, int32_t rangeBegin, int32_t rangeEnd, uint32_t *fullSize);
        Threading::Task<std::vector<LeaderboardInfo>> getLeaderboards();

        struct LeaderboardCache {
            Leaderboard mBoard;
            std::chrono::steady_clock::time_point mLastUpdate;
        };
        struct LeaderboardDescriptor {
            std::string mName;
            Leaderboard::AccessMode mAccessMode;

            auto operator<=>(const LeaderboardDescriptor &) const = default;
        };
        std::map<LeaderboardDescriptor, LeaderboardCache> mCache;

        /////////// STATS

        virtual Threading::TaskFuture<bool> ingestStat(const char *name, const char *leaderboardName, int32_t value) override;

        Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value);

        //////////////////

        bool mInitialized = false;

        EOS_HPlatform mPlatformHandle = nullptr;
        EOS_HAuth mAuthInterface = nullptr;
        EOS_HConnect mConnectInterface = nullptr;
        EOS_HLeaderboards mLeaderboardsInterface = nullptr;
        EOS_HStats mStatsInterface = nullptr;

#if EOS_LOGIN_METHOD_CREDENTIALS
        bool mRetryAccountOnFailure = false;
#endif

        Threading::TaskQueue mQueue;

        Threading::TaskFuture<EOS_EpicAccountId> mUserId;
        Threading::TaskFuture<EOS_ProductUserId> mProductUserId;
    };

}
}

REGISTER_TYPE(Engine::FirstParty::EpicServices)