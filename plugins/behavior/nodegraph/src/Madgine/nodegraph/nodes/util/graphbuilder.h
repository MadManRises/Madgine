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
    /*
    template <uint32_t Node, uint32_t Index, uint32_t Group = 0>
    struct SenderConnection {
        using value_type = SenderConnection;
        static constexpr uint32_t node = Node;

        static constexpr Pin pin(NodeGraph &graph, std::vector<NodeBase *> &nodes)
        {
            return { Node > 0 ? graph.nodeIndex(nodes[Node - 1]) : 0, Index, Group };
        }

        using next = SenderConnection<Node, Index + 1, Group>;
        using next_group = SenderConnection<Node, Index, Group + 1>;
    };

    template <typename T>
    struct isConnectionHelper : std::false_type {
    };

    template <uint32_t Node, uint32_t Index, uint32_t Group>
    struct isConnectionHelper<SenderConnection<Node, Index, Group>> : std::true_type {
    };

    template <typename T>
    concept IsConnection = isConnectionHelper<std::remove_reference_t<T>>::value;

    template <typename T, uint32_t Node, uint32_t Index, uint32_t Group>
    SenderConnection<Node, Index, Group> operator+(SenderConnection<Node, Index, Group>, T &&t);

    template <typename T, uint32_t Node, uint32_t Index, uint32_t Group>
    requires(!IsConnection<std::remove_reference_t<T>>)
        SenderConnection<Node, Index, Group>
    operator+(T &&t, SenderConnection<Node, Index, Group>);

    template <typename TypedTraits>
    using NodeHelper = SenderNode<typename TypedTraits::Algorithm>;

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

    template <typename TypedTraits>
    static constexpr bool hasNode = !std::same_as<typename TypedTraits::Algorithm, Execution::just_t> && !std::same_as<typename TypedTraits::Algorithm, Execution::let_value_t>;

    template <uint32_t baseIndex>
    struct helper {
        template <size_t I>
        using type = SenderConnection<baseIndex, I, 0>;
    };

    template <typename Algorithm, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers, typename... T>
    static auto argsHelper2(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<T...>)
    {
        ([&]() {
            if constexpr (!IsConnection<T>) {
                std::unique_ptr<SenderNode<Execution::just_t>> uNode = std::make_unique<SenderNode<Execution::just_t>>(graph);
                uNode->setArguments<0>(ValueType { std::string { "Foo" } });
                NodeBase *node = graph.addNode(std::move(uNode));
                parent.mNodes.push_back(node);
            }
        }(),
            ...);

        return ConnectionData<baseIndex + (!IsConnection<T> + ... + 0), FlowIn, typename Providers::template append<std::conditional_t<IsConnection<T>, T, SenderConnection<baseIndex, 0, 0>>...>> { std::move(parent) };
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

    template <uint32_t nodeIndex, uint32_t groupIndex, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto subHelper2(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<>)
    {
        return parent;
    }

    template <uint32_t nodeIndex, uint32_t groupIndex, uint32_t pinIndex, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto subHelper3(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<>)
    {
        return parent;
    }

    template <typename T>
    struct is_typed_algorithm_helper : std::false_type {
    };
    template <typename F, uint32_t argCount>
    struct is_typed_algorithm_helper<Execution::typed_algorithm<F, argCount>> : std::true_type {
    };
    template <typename T>
    concept is_typed_algorithm = is_typed_algorithm_helper<T>::value;

    template <typename F>
    struct subHelper5 {
        template <typename... T>
        using type = std::invoke_result_t<F, T...>;
    };

    template <uint32_t nodeIndex, uint32_t groupIndex, typename Providers, typename T>
    static auto subHelper4(T t)
    {
        if constexpr (is_typed_algorithm<T>) {
            if constexpr (groupIndex == 0) {
                return std::declval<typename Providers::template instantiate<subHelper5<typename T::F>::template type>>();
            } else {
                return []<size_t... Is>(std::index_sequence<Is...>)
                {
                    return std::declval<std::invoke_result_t<typename T::F, SenderConnection<nodeIndex, Is, groupIndex> &...>>();
                }
                (std::make_index_sequence<T::argCount> {});
            }
        } else {
            return std::declval<T>();
        }
    }

    template <uint32_t nodeIndex, uint32_t groupIndex, uint32_t pinIndex, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers, typename T, typename... Ty>
    static auto subHelper3(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<T, Ty...>)
    {
        using Sender = decltype(subHelper4<nodeIndex, groupIndex, Providers>(std::declval<T>()));
        ConnectionData sub = graphBuilderFromSender<Sender>(graph,
            ConnectionData<
                baseIndex,
                std::conditional_t<groupIndex == 0, FlowIn, SenderConnection<nodeIndex, pinIndex, groupIndex>>,
                type_pack<>,
                type_pack<>> { std::move(parent) });
        return subHelper3<nodeIndex, groupIndex, pinIndex + 1>(graph, std::move(sub), type_pack<Ty...> {});
    }

    template <uint32_t nodeIndex, uint32_t groupIndex, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers, typename T, typename... Ty>
    static auto subHelper2(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent, type_pack<T, Ty...>)
    {
        ConnectionData sub = subHelper3<nodeIndex, groupIndex, 0>(graph, std::move(parent), make_type_pack_t<T> {});
        return subHelper2<nodeIndex, groupIndex + 1>(graph, std::move(sub), type_pack<Ty...> {});
    }

    template <typename TypedTraits, uint32_t nodeIndex, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto subHelper(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent)
    {
        if constexpr (requires { typename TypedTraits::subgraphs; }) {
            return subHelper2<nodeIndex, hasNode<TypedTraits>>(graph, std::move(parent), typename TypedTraits::subgraphs {});
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

    template <typename TypedTraits>
    static void nameHelper(NodeHelper<TypedTraits> &node)
    {
        if constexpr (requires { typename TypedTraits::NameMappings; }) {
            [&]<fixed_string... From, fixed_string... To>(type_pack<Execution::name_mapping<From, To>...>) {
                (node.template setDynamicName<From>(To), ...);
            }(typename TypedTraits::NameMappings {});
        }
    }

    template <typename TypedTraits, uint32_t baseIndex, typename FlowIn, typename Providers, typename Receivers>
    static auto nodeHelper(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent)
    {
        if constexpr (hasNode<TypedTraits>) {
            std::unique_ptr<NodeHelper<TypedTraits>> uNode = std::make_unique<NodeHelper<TypedTraits>>(graph);
            nameHelper<TypedTraits>(*uNode);
            NodeBase *node = graph.addNode(std::move(uNode));
            parent.mNodes.push_back(node);
            assert(parent.mNodes.size() == decltype(parent)::index);

            using in_types = typename Execution::sender_traits<typename TypedTraits::Algorithm>::argument_types::template filter<is_pred_sender>::template unpack_unique<Execution::pred_sender<Execution::signature<>>>::Signature;
            static_assert(in_types::count == decltype(parent)::Providers::size || (in_types::count <= decltype(parent)::Providers::size &&in_types::variadic));
            connectHelper<Execution::sender_traits<typename TypedTraits::Algorithm>>(graph, parent, graph.nodeIndex(node), std::make_index_sequence<decltype(parent)::Providers::size> {});

            ConnectionData incremented = ConnectionData<decltype(parent)::index + 1, typename decltype(parent)::FlowIn, typename decltype(parent)::Providers, type_pack<>> { std::move(parent) };

            ConnectionData erased = ConnectionData<decltype(incremented)::index, typename decltype(incremented)::FlowIn, type_pack<>, type_pack<>> { std::move(incremented) };

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

        ConnectionData sub = subHelper < TypedTraits, hasNode<TypedTraits> ? nodeIndex : decltype(node)::FlowIn::node > (graph, std::move(node));

        if constexpr (hasNode<TypedTraits>) {
            return ConnectionData<
                decltype(sub)::index,
                std::conditional_t<Execution::sender_traits<typename TypedTraits::Algorithm>::constant, typename decltype(sub)::FlowIn, SenderConnection<nodeIndex, 0, 0>>,
                typename NodeHelper<TypedTraits>::Sender::template value_types<type_pack>::template transform_index<helper<nodeIndex>::template type>::template concat<typename decltype(sub)::Providers>, type_pack<>> {
                std::move(sub)
            };
        } else
            return sub;
    }

    template <typename Sender, uint32_t baseIndex = 0, typename FlowIn = SenderConnection<0, 0, 0>, typename Providers = type_pack<>, typename Receivers = type_pack<>>
    auto graphBuilderFromSender(NodeGraph &graph, ConnectionData<baseIndex, FlowIn, Providers, Receivers> parent = {})
    {
        using Traits = Execution::typed_sender_traits<Sender>;

        return generate<Traits>(graph, std::move(parent));
    }*/
}
}