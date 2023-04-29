#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"
#include "../../nodeexecution.h"

namespace Engine {
namespace NodeGraph {

    template <uint32_t Node, uint32_t Index>
    struct SenderConnection {
        using value_type = SenderConnection;
        static constexpr uint32_t node = Node;

        static constexpr Pin pin(NodeGraph &graph, std::vector<NodeBase *> &nodes)
        {
            return { Node > 0 ? graph.nodeIndex(nodes[Node - 1]) : 0, Index };
        }

        using next = SenderConnection<Node, Index + 1>;
    };

    template <typename T>
    struct isConnectionHelper : std::false_type {
    };

    template <uint32_t Node, uint32_t Index>
    struct isConnectionHelper<SenderConnection<Node, Index>> : std::true_type {
    };

    template <typename T>
    concept IsConnection = isConnectionHelper<std::remove_reference_t<T>>::value;

    template <typename T, uint32_t Node, uint32_t Index>
    SenderConnection<Node, Index> operator+(SenderConnection<Node, Index>, T &&t)
    {
    }

    template <typename T, uint32_t Node, uint32_t Index>
    requires(!IsConnection<std::remove_reference_t<T>>)
        SenderConnection<Node, Index>
    operator+(T &&t, SenderConnection<Node, Index>)
    {
    }


    template <typename... T>
    struct signature {

        static constexpr size_t count = sizeof...(T);

        static ExtendedValueTypeDesc type(uint32_t index)
        {
            static constexpr ExtendedValueTypeDesc types[] = {
                toValueTypeDesc<T>()...
            };
            return types[index];
        }
    };

    template <typename Algorithm>
    struct sender_traits;

    template <>
    struct sender_traits<Execution::for_each_t> {
        static constexpr bool constant = false;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<std::vector<int>, algorithm<signature<ValueType>>>;
        using in_types = signature<>;

        using Sender = Execution::for_each_t::sender<std::vector<int>, NodeAlgorithm<1>>;

        static auto buildSender(NodeResults &results, std::vector<int> v)
        {
            return Sender {
                v, NodeAlgorithm<1> { results }
            };
        }
    };

    template <>
    struct sender_traits<Execution::let_value_t> {
        static constexpr bool constant = false;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<algorithm<signature<ValueType>>>;
        using in_types = signature<ValueType>;

        using Sender = Execution::let_value_t::sender<NodeReader<ValueType>, NodeAlgorithm<1>>;

        static auto buildSender(NodeResults &results)
        {
            return Sender { {},
                NodeReader<ValueType> {},
                NodeAlgorithm<1> { results } };
        }
    };

    template <>
    struct sender_traits<Execution::Variable_t> {
        static constexpr bool constant = true;
        static constexpr bool eagerNode = true;
        using argument_types = type_pack<ValueType>;
        using in_types = signature<>;

        using Sender = Execution::Variable_t::sender<"test", Execution::just_t::sender<>, ValueType>;

        using exposedVariables = type_pack<auto_holder<fixed_string { "test" }>>;

        static auto buildSender(NodeResults &results, ValueType value)
        {
            return Sender { {}, Execution::just(), value };
        }
    };

    template <>
    struct sender_traits<Execution::read_var_t> {
        static constexpr bool constant = true;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<>;
        using in_types = signature<>;

        using Sender = Execution::read_var_t::sender<int &, "test">;

        static auto buildSender(NodeResults &results)
        {
            return Sender {};
        }
    };

    template <>
    struct sender_traits<Execution::write_var_t> {
        static constexpr bool constant = false;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<>;
        using in_types = signature<ValueType>;

        using Sender = Execution::write_var_t::sender<NodeReader<ValueType>, "test">;

        static auto buildSender(NodeResults &results)
        {
            return Sender { {}, NodeReader<ValueType> {} };
        }
    };

    template <>
    struct sender_traits<Execution::then_t> {
        static constexpr bool constant = true;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<>;
        using in_types = signature<ValueType, ValueType>;

        static constexpr auto test = [](auto &&...v) { return (v + ...); };

        using Sender = Execution::then_t::sender<NodeReader<int, int>, decltype(test)>;

        static auto buildSender(NodeResults &results)
        {
            return Sender { {}, NodeReader<int, int> {} };
        }
    };

    template <>
    struct sender_traits<Execution::sequence_t> {
        static constexpr bool constant = false;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<algorithm<signature<>>, algorithm<signature<>>>;
        using in_types = signature<>;

        using Sender = Execution::sequence_t::sender<NodeSender<1>, NodeSender<2>>;

        static auto buildSender(NodeResults &results)
        {
            return Sender { {}, NodeSender<1> {}, NodeSender<2> {} };
        }
    };

    template <>
    struct sender_traits<Execution::just_t> {
        static constexpr bool constant = true;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<>;
        using in_types = signature<>;

        using Sender = Execution::just_t::sender<ValueType>;

        static auto buildSender(NodeResults &results)
        {
            return Sender { ValueType { 3 } };
        }
    };

    template <>
    struct sender_traits<Execution::assign_t> {
        static constexpr bool constant = false;
        static constexpr bool eagerNode = false;
        using argument_types = type_pack<>;
        using in_types = signature<ValueType>;

        using Sender = Execution::assign_t::sender<NodeReader<ValueType>, ValueType>;

        static auto buildSender(NodeResults &results)
        {
            return Sender { {}, NodeReader<ValueType> {}, *new ValueType };
        }
    };

    template <typename Sender>
    struct typed_sender_traits;

    template <typename C, typename F>
    struct typed_sender_traits<Execution::for_each_t::sender<C, F>> {
        static constexpr bool hasNode = true;
        using Algorithm = Execution::for_each_t;

        template <uint32_t nodeIndex>
        struct subgraphs {
            template <typename... Providers>
            using type = type_pack<std::invoke_result_t<F, SenderConnection<nodeIndex, 0> &>>;
        };
    };

    template <fixed_string Name, typename Sender, typename T>
    struct typed_sender_traits<Execution::Variable_t::sender<Name, Sender, T>> {
        using Algorithm = Execution::Variable_t;

        using Pred = Sender;

        static constexpr bool hasNode = true;
    };

    template <typename Sender, typename F>
    struct typed_sender_traits<Execution::let_value_t::sender<Sender, F>> {
        using Algorithm = Execution::let_value_t;

        using Pred = Sender;

        static constexpr bool hasNode = false;

        template <uint32_t nodeIndex>
        struct subgraphs {
            template <typename... Providers>
            using type = type_pack<std::invoke_result_t<F, Providers...>>;
        };
    };

    template <typename T, fixed_string Name>
    struct typed_sender_traits<Execution::read_var_t::sender<T, Name>> {
        using Algorithm = Execution::read_var_t;

        static constexpr bool hasNode = true;
    };

    template <typename Sender, fixed_string Name>
    struct typed_sender_traits<Execution::write_var_t::sender<Sender, Name>> {
        using Algorithm = Execution::write_var_t;

        using Pred = Sender;
        static constexpr bool hasNode = true;
    };

    template <typename Sender1, typename Sender2>
    struct typed_sender_traits<Execution::sequence_t::sender<Sender1, Sender2>> {
        using Algorithm = Execution::sequence_t;

        template <uint32_t nodeIndex>
        struct subgraphs {
            template <typename... Providers>
            using type = type_pack<Sender1, Sender2>;
        };

        static constexpr bool hasNode = true;
    };

    template <typename Sender, typename T>
    struct typed_sender_traits<Execution::then_t::sender<Sender, T>> {
        using Algorithm = Execution::then_t;
        using Pred = Sender;
        static constexpr bool hasNode = true;
    };

    template <typename... T>
    struct typed_sender_traits<Execution::just_t::sender<T...>> {
        using Algorithm = Execution::just_t;

        using argument_providers = type_pack<T...>;

        static constexpr bool hasNode = false;
    };

    template <typename Sender, typename T>
    struct typed_sender_traits<Execution::assign_t::sender<Sender, T>> {
        using Algorithm = Execution::assign_t;
        using Pred = Sender;
        static constexpr bool hasNode = true;
    };

}
}