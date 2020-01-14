#pragma once

#include "../toolscollector.h"

namespace Engine {
namespace Tools {

    struct Profiler : Tool<Profiler> {
        Profiler(ImRoot &root);

        virtual void render() override;

        const char *key() const override;

    private:
        Debug::Profiler::Profiler &mProfiler;
    };

}
}
