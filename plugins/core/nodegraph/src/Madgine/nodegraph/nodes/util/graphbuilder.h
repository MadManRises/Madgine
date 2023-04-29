#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "../../nodecollector.h"
#include "../../nodeinterpreter.h"
#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/state.h"

#include "sendernode.h"

namespace Engine {
namespace NodeGraph {

    template <uint32_t baseIndex, typename _FlowIn, typename _Providers = type_pack<>, typename Writes = type_pack<>>
    struct ConnectionData {

        static constexpr uint32_t index = baseIndex;
        using FlowIn = _FlowIn;
        using Providers = _Providers;

        ConnectionData(std::vector<NodeBase *> nodes = {})
            : mNodes(std::move(nodes))
        {
        }

        template <uint32_t otherIndex, typename OtherFlowIn, typename OtherProviders, typename OtherWrites>
        ConnectionData(ConnectionData<otherIndex, OtherFlowIn, OtherProviders, OtherWrites> &&other)
            : mNodes(std::move(other.mNodes))
        {
        }

        ConnectionData(const ConnectionData &) = delete;

        Pin flowInPin(NodeGraph &graph)
        {
            return FlowIn::pin(graph, mNodes);
        }

        template <size_t I>
        Pin provider(NodeGraph &graph)
        {
            return Providers::template select<I>::pin(graph, mNodes);
        }

        std::vector<NodeBase *> mNodes;
    };

    template <uint32_t baseIndex>
    struct helper {
        template <size_t I>
        using type = SenderConnection<baseIndex, I>;
    };

    template <typename Algorithm, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers, typename... T>
    static auto argsHelper2(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<T...>)
    {
        ([&]() {
            if constexpr (!IsConnection<T>) {
                std::unique_ptr<SenderNode<Algorithm>> uNode = std::make_unique<SenderNode<Algorithm>>(graph);
                NodeBase *node = graph.addNode(std::move(uNode));
                parent.mNodes.push_back(node);
            }
        }(),
            ...);

        return ConnectionData<baseIndex + (!IsConnection<T> + ... + 0), FlowIn, typename Providers::append<std::conditional_t<IsConnection<T>, T, SenderConnection<baseIndex, 0>>...>> { std::move(parent) };
    }

    template <typename TypedTraits, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto argsHelper(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent)
    {
        if constexpr (requires { typename TypedTraits::argument_providers; }) {
            return argsHelper2<typename TypedTraits::Algorithm>(graph, std::move(parent), typename TypedTraits::argument_providers {});
        } else {
            return parent;
        }
    }

    template <typename TypedTraits, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto predHelper(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent)
    {
        if constexpr (requires { typename TypedTraits::Pred; }) {
            return graphBuilderFromSender<typename TypedTraits::Pred>(graph, std::move(parent));
        } else {
            return parent;
        }
    }

    template <typename Connection, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto subHelper2(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<>)
    {
        return parent;
    }

    template <typename Connection, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers, typename T, typename... Ty>
    static auto subHelper2(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<T, Ty...>)
    {
        ConnectionData sub = graphBuilderFromSender<T>(graph,
            ConnectionData<
                baseIndex,
                Connection,
                type_pack<>,
                type_pack<>> { std::move(parent) });
        return subHelper2<typename Connection::next>(graph, std::move(sub), type_pack<Ty...> {});
    }

    template <typename TypedTraits, uint32_t nodeIndex, uint32_t startIndex, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto subHelper(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent)
    {
        if constexpr (requires { typename TypedTraits::template subgraphs<nodeIndex>; }) {
            return subHelper2<std::conditional_t<TypedTraits::hasNode, SenderConnection<nodeIndex, startIndex>, FlowIn>>(graph, std::move(parent), typename Providers::template instantiate<TypedTraits::template subgraphs<nodeIndex>::template type> {});
        } else {
            return parent;
        }
    }

    template <typename Traits, size_t... Is>
    static void connectHelper(NodeGraph &graph, auto &pred, uint32_t node, std::index_sequence<Is...>)
    {
        if constexpr (!Traits::constant)
            graph.connectFlow(pred.flowInPin(graph), { node, 0 });
        (graph.connectDataIn({ node, Is }, pred.template provider<Is>(graph)), ...);
    }

    template <typename TypedTraits, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto nodeHelper(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent)
    {
        if constexpr (TypedTraits::hasNode) {
            std::unique_ptr<NodeBase> uNode = std::make_unique<SenderNode<TypedTraits::Algorithm>>(graph);
            NodeBase *node = graph.addNode(std::move(uNode));
            parent.mNodes.push_back(node);
            assert(parent.mNodes.size() == decltype(parent)::index);

            static_assert(sender_traits<TypedTraits::Algorithm>::in_types::count == decltype(parent)::Providers::size);
            connectHelper<sender_traits<TypedTraits::Algorithm>>(graph, parent, graph.nodeIndex(node), std::make_index_sequence<sender_traits<TypedTraits::Algorithm>::in_types::count> {});

            ConnectionData incremented = ConnectionData<decltype(parent)::index + 1, decltype(parent)::FlowIn, decltype(parent)::Providers, type_pack<>> { std::move(parent) };

            ConnectionData erased = ConnectionData<decltype(incremented)::index, decltype(incremented)::FlowIn, type_pack<>, type_pack<>> { std::move(incremented) };

            return erased;
        } else {
            return parent;
        }
    }

    template <typename TypedTraits, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto generate(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent)
    {
        ConnectionData pred = predHelper<TypedTraits>(graph, std::move(parent));

        ConnectionData args = argsHelper<TypedTraits>(graph, std::move(pred));

        constexpr uint32_t nodeIndex = decltype(args)::index;

        ConnectionData node = nodeHelper<TypedTraits>(graph, std::move(args));

        ConnectionData sub = subHelper < TypedTraits, TypedTraits::hasNode ? nodeIndex : decltype(node)::FlowIn::node, TypedTraits::hasNode > (graph, std::move(node));

        if constexpr (TypedTraits::hasNode) {
            return ConnectionData<
                decltype(sub)::index,
                std::conditional_t<sender_traits<TypedTraits::Algorithm>::constant, decltype(sub)::FlowIn, SenderConnection<nodeIndex, 0>>,
                typename sender_traits<TypedTraits::Algorithm>::Sender::value_types<type_pack>::transform_index<helper<nodeIndex>::template type>::concat<typename decltype(sub)::Providers>, type_pack<>> {
                std::move(sub)
            };
        } else
            return sub;
    }

    template <typename Sender, uint32_t baseIndex = 0, typename FlowIn = SenderConnection<0, 0>, typename Providers = type_pack<>, typename Receivers = type_pack<>>
    auto graphBuilderFromSender(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent = {})
    {
        using Traits = typed_sender_traits<Sender>;

        return generate<Traits>(graph, std::move(parent));
    }
}
}