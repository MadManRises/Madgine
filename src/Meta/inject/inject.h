#pragma once

#include "context.h"

namespace Engine {
namespace Inject {

    template <typename... Injectors, typename F>
    auto inject(F *f)
    {
        auto context = make_context<Injectors...>();

        f();

        return context;
    }

}
}