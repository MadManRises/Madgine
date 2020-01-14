#pragma once

#include "processstats.h"

namespace Engine {
namespace Debug {
    namespace Profiler {

        struct MODULES_EXPORT StaticProcess {
            StaticProcess(const char *function)
                : mStats(function)
            {
            }

            void start();
            void stop();

        private:
            ProcessStats mStats;

            ProcessStats *mPrevious = nullptr;

            std::pair<ProcessStats *const, ProcessStats::Data> *mParent = nullptr;
        };

    }
}
}
