#pragma once

namespace Engine {
namespace Execution {

    template <typename _Signature>
    struct algorithm {
        using Signature = _Signature;
    };
    template <typename _Signature>
    struct pred_sender {
        using Signature = _Signature;
    };
    struct succ_sender;

    template <typename T>
    struct recursive {
        using decay_t = T;
    };

    template <typename T>
    using is_recursive = is_instance<T, recursive>;

    template <typename... T>
    struct signature : type_pack<T...> {

        static constexpr size_t count = sizeof...(T);

        using recursive_t = decayed_t<typename type_pack<T...>::template filter<is_recursive>::template unpack_unique<void>>;
        using non_recursive_arguments = typename type_pack<T...>::template filter<Not<is_recursive>::type>;

        static constexpr bool variadic = !std::same_as<recursive_t, void>;

        static constexpr size_t variadicIndex = []() {
            if constexpr (variadic) {
                return type_pack<T...>::template index<size_t, recursive<recursive_t>>;
            } else {
                return std::numeric_limits<size_t>::max();
            }
        }();

    };

    template <typename Algorithm>
    struct sender_traits;

    template <>
    struct sender_traits<Execution::for_each_t> {
        static constexpr bool constant = false;
        using argument_types = type_pack<std::vector<int>, algorithm<signature<ValueType>>>;

        static constexpr auto &algorithm = Execution::for_each;
    };

    template <>
    struct sender_traits<Execution::let_value_t> {
        static constexpr bool constant = false;
        using argument_types = type_pack<pred_sender<signature<ValueType>>, algorithm<signature<ValueType>>>;

        static constexpr auto &algorithm = Execution::let_value;
    };

    template <>
    struct sender_traits<Execution::Variable_t> {
        static constexpr bool constant = true;
        using argument_types = type_pack<Execution::just_t::sender<>, ValueType>;

        static constexpr auto &algorithm = Execution::Variable<"Name">;

        using exposedVariables = type_pack<auto_holder<fixed_string { "Name" }>>;
    };

    template <>
    struct sender_traits<Execution::read_var_t> {
        static constexpr bool constant = true;
        using argument_types = type_pack<>;

        static constexpr auto &algorithm = Execution::read_var<int &, "Name">;
    };

    template <>
    struct sender_traits<Execution::write_var_t> {
        static constexpr bool constant = false;
        using argument_types = type_pack<pred_sender<signature<ValueType>>>;

        static constexpr auto &algorithm = Execution::write_var<"Name">;
    };

    template <>
    struct sender_traits<Execution::sequence_t> {
        static constexpr bool constant = false;
        using argument_types = type_pack<>;
        using variadic = type_pack<succ_sender>;

        static constexpr auto &algorithm = Execution::sequence;
    };

    template <>
    struct sender_traits<Execution::just_t> {
        static constexpr bool constant = true;
        using argument_types = type_pack<ValueType>;

        static constexpr auto &algorithm = Execution::just;
    };

    template <>
    struct sender_traits<Execution::assign_t> {
        static constexpr bool constant = false;
        using argument_types = type_pack<pred_sender<signature<ValueType>>, ValueType>;

        static constexpr auto &algorithm = Execution::assign;
    };

    template <typename Sender>
    struct typed_sender_traits;

    template <typename _F, uint32_t _argCount>
    struct typed_algorithm {
        using F = _F;
        static constexpr uint32_t argCount = _argCount;
    };

    template <typename C, typename F>
    struct typed_sender_traits<Execution::for_each_t::sender<C, F>> {
        using Algorithm = Execution::for_each_t;

        using subgraphs = type_pack<typed_algorithm<F, 1>>;
    };

    template <fixed_string Name, typename Sender, typename T>
    struct typed_sender_traits<Execution::Variable_t::sender<Name, Sender, T>> {
        using Algorithm = Execution::Variable_t;

        using Pred = Sender;
    };

    template <typename Sender, typename F>
    struct typed_sender_traits<Execution::let_value_t::sender<Sender, F>> {
        using Algorithm = Execution::let_value_t;

        using Pred = Sender;

        using subgraphs = type_pack<typed_algorithm<F, 1>>;
    };

    template <typename T, fixed_string Name>
    struct typed_sender_traits<Execution::read_var_t::sender<T, Name>> {
        using Algorithm = Execution::read_var_t;
    };

    template <typename Sender, fixed_string Name>
    struct typed_sender_traits<Execution::write_var_t::sender<Sender, Name>> {
        using Algorithm = Execution::write_var_t;

        using Pred = Sender;
    };

    template <typename... Sender>
    struct typed_sender_traits<Execution::sequence_t::sender<Sender...>> {
        using Algorithm = Execution::sequence_t;

        using subgraphs = type_pack<type_pack<Sender...>>;
    };

    template <typename Sender, typename T>
    struct typed_sender_traits<Execution::then_t::sender<Sender, T>> {
        using Algorithm = Execution::then_t::typed<T>;
        using Pred = Sender;
    };

    template <typename... T>
    struct typed_sender_traits<Execution::just_t::sender<T...>> {
        using Algorithm = Execution::just_t;

        using argument_providers = type_pack<T...>;
    };

    template <typename Sender, typename T>
    struct typed_sender_traits<Execution::assign_t::sender<Sender, T>> {
        using Algorithm = Execution::assign_t;
        using Pred = Sender;
    };

}
}