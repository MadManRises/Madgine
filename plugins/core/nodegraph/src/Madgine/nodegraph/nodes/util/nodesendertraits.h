#pragma once

#include "Generic/execution/sendertraits.h"

#include "functors.h"

namespace Engine {
namespace Execution {

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Add>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<recursive<int>>>>;
        using variadic = type_pack<pred_sender<signature<recursive<int>, int>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::Add>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Log>> {
        static constexpr bool constant = false;
        using argument_types = type_pack<pred_sender<signature<ValueType>>>;
        using variadic = type_pack<pred_sender<signature<ValueType>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::Log>;
    };

}
}