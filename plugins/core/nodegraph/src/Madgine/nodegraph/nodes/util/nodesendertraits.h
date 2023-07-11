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
    struct sender_traits<then_t::typed<NodeGraph::Divide>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<recursive<Vector3>>>>;
        using variadic = type_pack<pred_sender<signature<recursive<Vector3>, float>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::Divide>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Log>> {
        static constexpr bool constant = false;
        using argument_types = type_pack<pred_sender<signature<ValueType>>>;
        using variadic = type_pack<pred_sender<signature<ValueType>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::Log>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Vector3To4>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<Vector3, float>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::Vector3To4>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::BreakVector3>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<Vector3>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::BreakVector3>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::BreakVector4>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<Vector4>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::BreakVector4>;
    };
        
    template <>
    struct sender_traits<then_t::typed<NodeGraph::MakeVector3>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<float, float, float>>>;

        static constexpr auto &algorithm = typed_then<NodeGraph::MakeVector3>;
    };

}
}