#pragma once

#include "../../generic/builder.h"

namespace Engine {
namespace Serialize {

    template <typename Builder>
    struct RequestBuilder : Builder{        

        using Builder::Builder;

        template <typename C>
        auto then(C &&c)
        {
            return this->template append<0>(std::forward<C>(c));
        }
        
        template <typename C>
        auto onSuccess(C &&c)
        {
            return this->template append<1>(std::forward<C>(c));
        }

        template <typename C>
        auto onFailure(C &&c)
        {
            return this->template append<2>(std::forward<C>(c));
        }
    };

    template <typename F>
    RequestBuilder(F &&f)->RequestBuilder<Builder<F, RequestBuilder, 3>>;

}
}