#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "../../nodecollector.h"
#include "../../nodeinterpreter.h"
#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/state.h"

#include "../../nodeexecution.h"
#include "../../nodegraph.h"
#include "../../pins.h"
#include "nodesendertraits.h"

namespace Engine {

namespace NodeGraph {

    template <typename T>
    using is_algorithm = is_instance<decayed_t<T>, Execution::algorithm>;

    template <typename T>
    using is_pred_sender = is_instance<decayed_t<T>, Execution::pred_sender>;

    template <typename T>
    using is_succ_sender = std::is_same<decayed_t<T>, Execution::succ_sender>;

    template <typename T>
    using is_value = std::negation<std::disjunction<is_algorithm<T>, is_pred_sender<T>, is_succ_sender<T>>>;

    template <typename... T>
    struct algorithm_argument_helper;

    template <>
    struct algorithm_argument_helper<> {
        static constexpr size_t count = 0;

        static ExtendedValueTypeDesc type(uint32_t index)
        {
            throw 0;
        }
    };

    template <typename Traits>
    auto variadic_helper()
    {
        if constexpr (requires { typename Traits::variadic; }) {
            return typename Traits::variadic {};
        } else {
            return type_pack<> {};
        }
    }

    template <typename Signature>
    static ExtendedValueTypeDesc signature_type(uint32_t index)
    {
        return [index]<typename... T>(type_pack<T...>)
        {
            static constexpr ExtendedValueTypeDesc types[] = {
                toValueTypeDesc<std::remove_reference_t<decayed_t<T>>>()...
            };
            if constexpr (Signature::variadic) {
                if (index >= Signature::count)
                    return toValueTypeDesc<typename Signature::recursive_t>();
            }
            return types[index];
        }
        (Signature {});
    }

    template <typename Algorithm>
    struct SenderNode : Node<SenderNode<Algorithm>> {

        using Traits = Execution::sender_traits<Algorithm>;

        using argument_types = typename Traits::argument_types;
        using algorithms = typename argument_types::template filter<is_algorithm>;
        using value_argument_tuple = typename argument_types::template filter<is_value>::template instantiate<std::tuple>;
        using in_types = typename argument_types::template filter<is_pred_sender>;
        using variadic = decltype(variadic_helper<Traits>());

        template <uint32_t I>
        static auto buildArgs(const std::tuple<> &values, type_pack<> args, std::vector<NodeResults> *results = nullptr, NodeResults *variadicBuffer = nullptr, size_t *recursiveOffset = nullptr)
        {
            return std::make_tuple();
        }

        template <uint32_t I, typename... Vs, typename T, typename... Ts>
        static auto buildArgs(std::tuple<Vs...> &&values, type_pack<T, Ts...> args, std::vector<NodeResults> *results = nullptr, NodeResults *variadicBuffer = nullptr, size_t *recursiveOffset = nullptr)
        {
            if constexpr (is_pred_sender<T>::value) {
                return std::tuple_cat(
                    std::make_tuple(typename T::Signature::template instantiate<NodeReader> { recursiveOffset, variadicBuffer }),
                    buildArgs<I>(std::move(values), type_pack<Ts...> {}, results, variadicBuffer, recursiveOffset));
            } else if constexpr (is_succ_sender<T>::value) {
                return std::tuple_cat(
                    std::make_tuple(NodeSender<I + 1> {}),
                    buildArgs<I + 1>(std::move(values), type_pack<Ts...> {}, results, variadicBuffer, recursiveOffset));
            } else if constexpr (is_algorithm<T>::value) {
                assert(results);
                return std::tuple_cat(
                    std::make_tuple(NodeAlgorithm<I + 1> { *results }),
                    buildArgs<I + 1>(std::move(values), type_pack<Ts...> {}, results, variadicBuffer, recursiveOffset));
            } else {
                return TupleUnpacker::prepend<first_t<Vs...>>(
                    std::get<0>(std::move(values)),
                    buildArgs<I>(TupleUnpacker::popFront(std::move(values)), type_pack<Ts...> {}, results, variadicBuffer));
            }
        }

        static auto buildSender(value_argument_tuple &&values, std::vector<NodeResults> *results = nullptr, NodeResults *variadicBuffer = nullptr)
        {
            return TupleUnpacker::invokeFromTuple(Traits::algorithm, buildArgs<0>(std::move(values), argument_types {}, results, variadicBuffer));
        }

        static auto buildVariadicSender(size_t *recursiveOffset, NodeResults *variadicBuffer = nullptr)
        {
            if constexpr (requires { typename Traits::variadic; }) {
                return TupleUnpacker::invokeFromTuple(Traits::algorithm, buildArgs<0>(std::make_tuple(), variadic {}, nullptr, variadicBuffer, recursiveOffset));
            }
        }

        using Sender = decltype(buildSender(std::declval<value_argument_tuple>()));
        using VariadicSender = decltype(buildVariadicSender(nullptr));

        struct DummyReceiver : NodeExecutionReceiver {
            template <typename... Args>
            void set_value(Args &&...args)
            {
                mValues = { ValueType { std::forward<Args>(args) }... };
                mHasReturned = true;
            }

            bool mHasReturned = false;
            NodeResults mValues;

            template <typename CPO>
            friend decltype(auto) tag_invoke(CPO f, DummyReceiver &receiver)
            {
                return f(static_cast<NodeExecutionReceiver &>(receiver));
            }
        };

        SenderNode(NodeGraph &graph)
            : Node<SenderNode<Algorithm>>(graph)
        {
            this->setup();
        }

        SenderNode(const SenderNode &other, NodeGraph &graph)
            : Node<SenderNode<Algorithm>>(graph)
        {
        }

        size_t flowInCount(uint32_t group) const override
        {
            return !Traits::constant;
        }

        std::string_view flowInName(uint32_t index, uint32_t group) const override
        {
            return "in";
        }

        size_t flowOutGroupCount() const override
        {
            constexpr size_t algorithm_count = argument_types::template filter<is_algorithm>::size;
            constexpr size_t succ_sender_count = argument_types::template filter<is_succ_sender>::size;
            constexpr size_t variadic_succ_sender_count = variadic::template filter<is_succ_sender>::size;

            return 1 + algorithm_count + succ_sender_count + variadic_succ_sender_count;
        }

        size_t flowOutBaseCount(uint32_t group) const override
        {
            static constexpr auto counts = []<typename... InnerAlg, typename... SuccSender, typename... VariadicSuccSender>(type_pack<InnerAlg...>, type_pack<SuccSender...>, type_pack<VariadicSuccSender...>)
            {
                return std::array {
                    static_cast<int>(!Traits::constant),
                    (sizeof(type_pack<InnerAlg>), 1)...,
                    (sizeof(type_pack<SuccSender>), 1)...,
                    (sizeof(type_pack<VariadicSuccSender>), 0)...
                };
            }
            (typename argument_types::template filter<is_algorithm> {}, typename argument_types::template filter<is_succ_sender> {}, typename variadic::template filter<is_succ_sender> {});
            return counts[group];
        }

        std::string_view flowOutName(uint32_t index, uint32_t group) const override
        {
            return "out";
        }

        bool flowOutVariadic(uint32_t group = 0) const override
        {
            if constexpr (requires { typename Traits::variadic; }) {
                return group == 1 && std::same_as<typename Traits::variadic::template unpack_unique<void>, Execution::succ_sender>;
            } else {
                return false;
            }
        }

        size_t dataInBaseCount(uint32_t group) const override
        {
            return in_types::template unpack_unique<Execution::pred_sender<Execution::signature<>>>::Signature::count;
        }

        ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group, bool bidir = true) const override
        {
            return signature_type<typename in_types::template unpack_unique<Execution::pred_sender<Execution::signature<>>>::Signature>(index);
        }

        bool dataInVariadic(uint32_t group = 0) const override
        {
            return group == 0 && in_types::template unpack_unique<Execution::pred_sender<Execution::signature<>>>::Signature::variadic;
        }

        size_t dataProviderGroupCount() const override
        {
            return 1 + algorithms::size;
        }

        size_t dataProviderBaseCount(uint32_t group) const override
        {
            static constexpr auto sizes = []<typename... InnerAlg>(type_pack<InnerAlg...>)
            {
                return std::array {
                    Sender::template value_types<type_pack>::size,
                    InnerAlg::Signature::count...
                };
            }
            (algorithms {});
            return sizes[group];
        }

        ExtendedValueTypeDesc dataProviderType(uint32_t index, uint32_t group, bool bidir = true) const override
        {
            static constexpr auto types = []<typename... InnerAlg>(type_pack<InnerAlg...>)
            {
                return std::array {
                    signature_type<typename Sender::template value_types<Execution::signature>>,
                    signature_type<typename InnerAlg::Signature>...
                };
            }
            (algorithms {});
            return types[group](index);
        }

        struct InterpretData : NodeInterpreterData {

            struct Receiver;

            using InnerReceiver = std::conditional_t<Traits::constant, DummyReceiver &, Receiver>;
            using State = Execution::connect_result_t<Sender, InnerReceiver>;

            InterpretData()
            {
                static_assert(!Traits::constant);
            }

            InterpretData(DummyReceiver &receiver, value_argument_tuple args)
            {
                static_assert(Traits::constant);
                buildState(receiver, std::move(args));
            }

            ~InterpretData()
            {
                if constexpr (Traits::constant) {
                    cleanup();
                }
            }

            struct Receiver : NodeReceiver {
                InterpretData *mData;

                template <typename... Args>
                void set_value(Args &&...args)
                {
                    if (mData->mResults.empty())
                        mData->mResults.emplace_back();
                    mData->mResults.front() = { ValueType { std::forward<Args>(args) }... };
                    mData->cleanup();
                    NodeReceiver::set_value();
                }

                void set_done()
                {
                    mData->cleanup();
                    NodeReceiver::set_done();
                }

                void set_error(GenericResult result)
                {
                    mData->cleanup();
                    NodeReceiver::set_error(result);
                }
            };

            struct RecursiveReceiver {
                InterpretData *mData;

                template <typename... Args>
                void set_value(Args &&...args)
                {
                    if (mData->mRecursiveBuffer.empty())
                        mData->mRecursiveBuffer.emplace_back();
                    mData->mRecursiveBuffer.front() = { ValueType { std::forward<Args>(args) }... };
                    mData->continueRecursion();
                }

                void set_done()
                {
                    if constexpr (!Traits::constant) {
                        mData->cleanup();
                    }
                    throw 0;
                }

                void set_error(GenericResult result)
                {
                    if constexpr (!Traits::constant) {
                        mData->cleanup();
                    }
                    throw 0;
                }

                template <typename CPO>
                friend decltype(auto) tag_invoke(CPO f, RecursiveReceiver &receiver)
                {
                    return f(receiver.mData->mState);
                }
            };

            void start(NodeReceiver receiver, value_argument_tuple args)
            {
                static_assert(!Traits::constant);
                buildState(Receiver { receiver, this }, std::move(args));
                start();
            }

            void start()
            {
                if constexpr (requires { typename Traits::variadic; }) {

                    using PredSender = typename argument_types::template filter<is_pred_sender>::template unpack_unique<Execution::pred_sender<Execution::signature<>>>;
                    if constexpr (PredSender::Signature::variadic) {
                        mRecursiveBuffer.resize(1);
                        Execution::get_context(mState).read(mRecursiveBuffer[0], PredSender::Signature::variadicIndex);
                    }

                    mVariadicInfo.mRecursionLevel = 0;
                    if (!mVariadicInfo.mStates.empty()) {
                        mVariadicInfo.mStates.front().mState.start();
                    } else {
                        mState.start();
                    }
                } else {
                    mState.start();
                }
            }

            void buildState(InnerReceiver receiver, value_argument_tuple &&args)
            {
                new (&mState) State { Execution::connect(buildSender(std::move(args), &mResults, &mRecursiveBuffer), std::forward<InnerReceiver>(receiver)) };

                if constexpr (requires { typename Traits::variadic; }) {

                    size_t baseIndex = 0;
                    uint32_t group = 1;

                    using PredSender = typename argument_types::template filter<is_pred_sender>::template unpack_unique<Execution::pred_sender<Execution::signature<>>>;
                    if constexpr (PredSender::Signature::variadic) {
                        baseIndex = PredSender::Signature::variadicIndex + 1;
                        group = 0;
                    }

                    mVariadicInfo.mStates.resize(Execution::get_context(receiver).mNode->variadicPinCount(group));
                    for (auto &state : mVariadicInfo.mStates) {
                        new (&state.mState) std::remove_reference_t<decltype(state.mState)> {
                            Execution::connect(buildVariadicSender(&baseIndex, &mRecursiveBuffer), RecursiveReceiver { this })
                        };
                    }
                }
            }

            void cleanup()
            {
                if constexpr (requires { typename Traits::variadic; }) {
                    while (mVariadicInfo.mRecursionLevel < mVariadicInfo.mStates.size()) {
                        auto &state = mVariadicInfo.mStates[mVariadicInfo.mRecursionLevel];
                        using State = std::remove_reference_t<decltype(state)>;
                        state.~State();
                    }
                    mVariadicInfo.mStates.clear();
                }
                mState.~State();
            }

            void continueRecursion()
            {
                if constexpr (!Traits::constant) {
                    auto &state = mVariadicInfo.mStates[mVariadicInfo.mRecursionLevel];
                    using State = std::remove_reference_t<decltype(state)>;
                    state.~State();
                }
                ++mVariadicInfo.mRecursionLevel;
                if (mVariadicInfo.mRecursionLevel < mVariadicInfo.mStates.size()) {
                    mVariadicInfo.mStates[mVariadicInfo.mRecursionLevel].mState.start();
                } else {
                    mState.start();
                }
            }

            virtual bool resolveVar(ValueType &result, std::string_view name) override
            {
                if constexpr (requires { typename Traits::exposedVariables; }) {
                    return [&]<fixed_string... Names>(type_pack<auto_holder<Names>...>)
                    {
                        return ([&]() {                    
                            if (name == Names) {
                                result = Execution::resolve_var<Names>(mState);
                                return true;
                            }
                            return false; }() || ...);
                    }
                    (typename Traits::exposedVariables {});
                }
                return false;
            }

            virtual std::map<std::string_view, ValueType> variables() override
            {
                if constexpr (requires { typename Traits::exposedVariables; }) {
                    return [&]<fixed_string... Names>(type_pack<auto_holder<Names>...>)
                    {
                        std::map<std::string_view, ValueType> result;
                        (result.try_emplace(Names, Execution::resolve_var<Names>(mState)), ...);
                        return result;
                    }
                    (typename Traits::exposedVariables {});
                } else
                    return {};
            }

            ValueType read(uint32_t providerIndex, uint32_t group) const
            {
                return mResults[group][providerIndex];
            }

            std::vector<NodeResults> mResults;
            NodeResults mRecursiveBuffer;
            union {
                State mState;
            };

            static auto variadicHelper()
            {
                if constexpr (requires { typename Traits::variadic; }) {
                    struct VariadicState {
                        VariadicState() { }
                        ~VariadicState() { }
                        union {
                            Execution::connect_result_t<VariadicSender, RecursiveReceiver> mState;
                        };
                    };
                    struct VariadicInfo {
                        std::vector<VariadicState> mStates;
                        size_t mRecursionLevel;
                    };
                    return VariadicInfo {};
                } else {
                    return std::monostate {};
                }
            }

            decltype(variadicHelper()) mVariadicInfo;
        };

        void setupInterpret(NodeInterpreter &interpreter, std::unique_ptr<NodeInterpreterData> &data) const override
        {
            if constexpr (requires { typename Traits::exposedVariables; }) {
                DummyReceiver rec { this, interpreter };
                data = std::make_unique<InterpretData>(rec, mArguments);
            }
        }

        void interpret(NodeReceiver receiver, std::unique_ptr<NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const override
        {
            if constexpr (!Traits::constant) {
                if (!data) {
                    data = std::make_unique<InterpretData>();
                }
                static_cast<InterpretData *>(data.get())->start(receiver, mArguments);
            } else {
                throw 0;
            }
        }

        void interpretRead(NodeInterpreter &interpreter, ValueType &retVal, std::unique_ptr<NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group) const override
        {
            if constexpr (Traits::constant) {

                DummyReceiver rec { this, interpreter };

                InterpretData data { rec, mArguments };

                data.start();

                assert(rec.mHasReturned);
                retVal = rec.mValues[providerIndex];
            } else {
                assert(data);
                retVal = static_cast<InterpretData *>(data.get())->read(providerIndex, group);
            }
        }

        value_argument_tuple mArguments;
        template <size_t I>
        auto &getArguments()
        {
            return std::get<I>(mArguments);
        }
        template <size_t I>
        void setArguments(std::tuple_element_t<I, value_argument_tuple> v)
        {
            std::get<I>(mArguments) = v;
        }
    };

}
}