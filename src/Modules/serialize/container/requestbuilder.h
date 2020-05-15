#pragma once

#include "../../generic/builder.h"

namespace Engine {
namespace Serialize {

    template <typename F, typename Builder = Builder<F, 3>>
    struct RequestBuilder : Builder{        

        template <typename F>
        auto onSuccess(F &&f) -> RequestBuilder<F, decltype(std::declval<Builder>().template append<1>(std::forward<F>(f)))>
        {
            return { this->template append<1>(std::forward<F>(f)) };
        }
    };

    template <typename F>
    RequestBuilder(F &&f)->RequestBuilder<F>;

}
}