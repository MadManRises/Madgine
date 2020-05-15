#pragma once

#include "../../generic/builder.h"

namespace Engine {
namespace Serialize {

    template <typename F, typename Builder = Builder<F, 3>>
    struct RequestBuilder : Builder{        

        template <typename C>
        auto onSuccess(C &&c) -> RequestBuilder<F, decltype(std::declval<Builder>().template append<1>(std::forward<C>(c)))>
        {
            return { this->template append<1>(std::forward<C>(c)) };
        }
    };

    template <typename F>
    RequestBuilder(F &&f)->RequestBuilder<F>;

}
}