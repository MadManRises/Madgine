#pragma once

#include "Madgine/root/rootcomponentcollector.h"

#include "Madgine/root/rootcomponentbase.h"

#include "Modules/threading/taskfuture.h"

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

    struct MADGINE_FIRST_PARTY_EXPORT FirstPartyServices : Root::VirtualRootComponentBase<FirstPartyServices> {

        FirstPartyServices(Root::Root &root);

        ///////// LEADERBOARD

        Threading::TaskFuture<Leaderboard> getLeaderboard(const char *name, Leaderboard::AccessMode accessmode = Leaderboard::GLOBAL, Leaderboard::ReferenceRank referenceRank = Leaderboard::FIRST, int32_t rangeBegin = 0, int32_t rangeEnd = 99, uint32_t *fullSize = nullptr);
        virtual Threading::Task<Leaderboard> getLeaderboardTask(const char *name, Leaderboard::AccessMode accessmode = Leaderboard::GLOBAL, Leaderboard::ReferenceRank referenceRank = Leaderboard::FIRST, int32_t rangeBegin = 0, int32_t rangeEnd = 99, uint32_t *fullSize = nullptr) = 0;

        ///////// STATS

        Threading::TaskFuture<bool> ingestStat(const char *name, const char *leaderboardName, int32_t value);
        virtual Threading::Task<bool> ingestStatTask(const char *name, const char *leaderboardName, int32_t value) = 0;

        ///////// ACHIEVEMENTS

        Threading::TaskFuture<bool> unlockAchievement(const char *name);
        virtual Threading::Task<bool> unlockAchievementTask(const char *name) = 0;
    };

    template <typename T>
    using FirstPartyServicesImpl = VirtualScope<T, Root::RootComponentVirtualImpl<T, FirstPartyServices>>;

}
}

REGISTER_TYPE(Engine::FirstParty::FirstPartyServices)