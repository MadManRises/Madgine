#pragma once

#include "Generic/execution/sendertraits.h"

#include "functors.h"

namespace Engine {
namespace Execution {

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Add>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<stream<int>>>>;

        using algorithm = then_t::typed<NodeGraph::Add>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Divide>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<stream<Vector3>>>>;

        using algorithm = then_t::typed<NodeGraph::Divide>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Log>> {
        static constexpr bool constant = false;
        using argument_types = type_pack<pred_sender<signature<ValueType>>>;

        using algorithm = then_t::typed<NodeGraph::Log>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::Vector3To4>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<Vector3, float>>>;

        using algorithm = then_t::typed<NodeGraph::Vector3To4>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::BreakVector3>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<Vector3>>>;

        using algorithm = then_t::typed<NodeGraph::BreakVector3>;
    };

    template <>
    struct sender_traits<then_t::typed<NodeGraph::BreakVector4>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<Vector4>>>;

        using algorithm = then_t::typed<NodeGraph::BreakVector4>;
    };
        
    template <>
    struct sender_traits<then_t::typed<NodeGraph::MakeVector3>> {
        static constexpr bool constant = true;
        using argument_types = type_pack<pred_sender<signature<float, float, float>>>;

        using algorithm = then_t::typed<NodeGraph::MakeVector3>;
    };

}
}