#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "../../nodecollector.h"
#include "../../nodeinterpreter.h"
#include "Generic/delayedconstruct.h"
#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/state.h"
#include "Generic/manuallifetime.h"

#include "../../nodeexecution.h"
#include "../../nodegraph.h"
#include "../../pins.h"
#include "automasknode.h"
#include "nodesendertraits.h"

#include "Madgine/codegen/fromsender.h"

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

    template <typename T, size_t>
    struct remove_deductors_impl {
        using type = T;
    };

    template <size_t I>
    struct dynamic_value_type : ValueType {
        static constexpr size_t index = I;
        using decay_t = ValueType;
        using ValueType::operator=;
        using no_value_type = void;
    };

    template <size_t I>
    struct remove_deductors_impl<ValueType, I> {
        using type = dynamic_value_type<I>;
    };

    template <typename T, size_t I>
    using remove_deductors = typename remove_deductors_impl<T, I>::type;

    template <typename Algorithm>
    struct SenderNode : Node<SenderNode<Algorithm>, AutoMaskNode<>> {

        using Traits = Execution::sender_traits<Algorithm>;

        using argument_types = typename Traits::argument_types;
        using algorithms = typename argument_types::template filter<is_algorithm>;
        using value_arguments = typename argument_types::template filter<is_value>;
        using value_argument_tuple = typename value_arguments::template transform_with_index<remove_deductors>::template instantiate<std::tuple>;
        using in_types = typename argument_types::template filter<is_pred_sender>;

        static constexpr bool isVariadic = requires { typename Traits::variadic; };
        static constexpr bool hasVariables = requires { typename Traits::exposedVariables; };
        static_assert(Traits::constant || !hasVariables);

        static auto variadic_helper()
        {
            if constexpr (isVariadic) {
                return typename Traits::variadic {};
            } else {
                return type_pack<> {};
            }
        }
        using variadic = decltype(variadic_helper());

        template <uint32_t I>
        static auto buildArgs(const std::tuple<>& values, type_pack<> args, std::vector<NodeResults>* results = nullptr, NodeResults* variadicBuffer = nullptr, size_t* recursiveOffset = nullptr)
        {
            return std::make_tuple();
        }

        template <uint32_t I, typename... Vs, typename T, typename... Ts>
        static auto buildArgs(std::tuple<Vs...>&& values, type_pack<T, Ts...> args, std::vector<NodeResults>* results = nullptr, NodeResults* variadicBuffer = nullptr, size_t* recursiveOffset = nullptr)
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

        template <auto...>
        using helper = ValueType;
        using explicit_algorithm = typename replace<typename Traits::algorithm>::template pattern_tagged<Execution::variable_name_tag, helper>;

        static auto buildSender(value_argument_tuple&& values, std::vector<NodeResults>* results = nullptr, NodeResults* variadicBuffer = nullptr)
        {
            return TupleUnpacker::invokeFromTuple(explicit_algorithm {}, buildArgs<0>(std::move(values), argument_types {}, results, variadicBuffer));
        }

        static auto buildVariadicSender(size_t* recursiveOffset, NodeResults* variadicBuffer = nullptr)
        {
            if constexpr (isVariadic) {
                return TupleUnpacker::invokeFromTuple(explicit_algorithm {}, buildArgs<0>(std::make_tuple(), typename Traits::variadic {}, nullptr, variadicBuffer, recursiveOffset));
            }
        }

        template <typename T>
        ExtendedValueTypeDesc resolveType() const
        {
            using decayedT = std::decay_t<T>;
            if constexpr (InstanceOfA<decayedT, Execution::variable_type>) {
                return this->mGraph.resolveVariableType(getDynamicName<is_instance_auto<decayedT, Execution::variable_type>::arguments::value>());
            } else if constexpr (InstanceOfA<decayedT, Execution::dynamic_argument_type>) {
                return getArguments<T::index>().type();
            } else if constexpr (InstanceOfA<decayedT, dynamic_value_type>) {
                return getArguments<T::index>().type();
            } else {
                return toValueTypeDesc<std::remove_reference_t<decayed_t<T>>>();
            }
        }

        template <typename Signature, typename Variadic = void>
        ExtendedValueTypeDesc signature_type(uint32_t index) const
        {
            if constexpr (Signature::variadic) {
                if (index >= Signature::count) {
                    static constexpr size_t variadicCount = Variadic::non_recursive_arguments::size;
                    static_assert(variadicCount > 0);
                    return [ this, index ]<typename... V>(type_pack<V...>)
                    {
                        ExtendedValueTypeDesc variadicTypes[] = {
                            resolveType<V>()...
                        };
                        return variadicTypes[(index - Signature::count) % variadicCount];
                    }
                    (typename Variadic::non_recursive_arguments {});
                }
            }
            return [ this, index ]<typename... T>(type_pack<T...>)
            {
                ExtendedValueTypeDesc types[] = {
                    resolveType<T>()...
                };
                return types[index];
            }
            (Signature {});
        }

        using Sender = decltype(buildSender(std::declval<value_argument_tuple>()));
        using VariadicSender = decltype(buildVariadicSender(nullptr));

        struct DummyReceiver : NodeExecutionReceiver<SenderNode<Algorithm>> {
            template <typename... Args>
            void set_value(Args&&... args)
            {
                mValues = { ValueType { std::forward<decayed_t<Args>>(args) }... };
                mHasReturned = true;
            }

            template <typename... Args>
            void set_error(Args&&... args)
            {
                throw 0;
            }

            bool mHasReturned = false;
            NodeResults mValues;

            template <typename CPO>
            friend decltype(auto) tag_invoke(CPO f, DummyReceiver& receiver)
            {
                return f(static_cast<NodeExecutionReceiver<SenderNode<Algorithm>>&>(receiver));
            }
        };

        SenderNode(NodeGraph& graph)
            : Node<SenderNode<Algorithm>, AutoMaskNode<>>(graph)
        {
            this->setup();
        }

        SenderNode(const SenderNode& other, NodeGraph& graph)
            : Node<SenderNode<Algorithm>, AutoMaskNode<>>(other, graph)
            , mArguments(other.mArguments)
            , mDynamicNames(other.mDynamicNames)
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
                    ((void)sizeof(type_pack<InnerAlg>), 1)...,
                    ((void)sizeof(type_pack<SuccSender>), 1)...,
                    ((void)sizeof(type_pack<VariadicSuccSender>), 0)...
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
            if constexpr (isVariadic) {
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
            return signature_type<typename in_types::template unpack_unique<Execution::pred_sender<Execution::signature<>>>::Signature, typename variadic::template filter<is_pred_sender>::template unpack_unique<Execution::pred_sender<Execution::signature<>>>::Signature>(index);
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
                    &SenderNode<Algorithm>::signature_type<typename Sender::template value_types<Execution::signature>>,
                    &SenderNode<Algorithm>::signature_type<typename InnerAlg::Signature>...
                };
            }
            (algorithms {});
            return (this->*types[group])(index);
        }

        bool resolveVariableType(ExtendedValueTypeDesc& type, std::string_view name) const override
        {
            if constexpr (hasVariables) {
                return [&]<fixed_string... Names, typename... T>(type_pack<Execution::variable<Names, T>...>)
                {
                    return ([&]() {
                        if (name == getDynamicName<Names>()) {
                            type = resolveType<T>();
                            return true;
                        }
                        return false;
                    }() || ...);
                }
                (typename Traits::exposedVariables {});
            }
            return false;
        }

        struct InterpretData : NodeInterpreterData {

            struct Receiver;

            using InnerReceiver = std::conditional_t<Traits::constant,
                std::conditional_t<hasVariables,
                    DummyReceiver,
                    DummyReceiver&>,
                Receiver>;
            using State = Execution::connect_result_t<Sender, InnerReceiver>;

            InterpretData()
            {
                static_assert(!Traits::constant);
            }

            InterpretData(InnerReceiver receiver, value_argument_tuple args)
            {
                static_assert(Traits::constant);
                buildState(std::forward<InnerReceiver>(receiver), std::move(args));
            }

            ~InterpretData()
            {
                if constexpr (Traits::constant) {
                    cleanup();
                }
            }

            struct Receiver : NodeReceiver<SenderNode<Algorithm>> {
                InterpretData* mData;

                template <typename... Args>
                void set_value(Args&&... args)
                {
                    if (mData->mResults.empty())
                        mData->mResults.emplace_back();
                    mData->mResults.front() = { ValueType { std::forward<Args>(args) }... };
                    mData->cleanup();
                    NodeReceiver<SenderNode<Algorithm>>::set_value();
                }

                void set_done()
                {
                    mData->cleanup();
                    NodeReceiver<SenderNode<Algorithm>>::set_done();
                }

                void set_error(GenericResult result)
                {
                    mData->cleanup();
                    NodeReceiver<SenderNode<Algorithm>>::set_error(result);
                }
            };

            struct RecursiveReceiver {
                InterpretData* mData;

                template <typename... Args>
                void set_value(Args&&... args)
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
                friend decltype(auto) tag_invoke(CPO f, RecursiveReceiver& receiver)
                {
                    return f(Execution::get_receiver(receiver.mData->mState));
                }
            };

            void start(NodeReceiver<SenderNode<Algorithm>> receiver, value_argument_tuple args)
            {
                static_assert(!Traits::constant);
                buildState(Receiver { receiver, this }, std::move(args));
                start();
            }

            void start()
            {
                if constexpr (isVariadic) {

                    using PredSender = typename argument_types::template filter<is_pred_sender>::template unpack_unique<Execution::pred_sender<Execution::signature<>>>;
                    if constexpr (PredSender::Signature::variadic) {
                        mRecursiveBuffer.resize(1);
                        Execution::get_context(Execution::get_receiver(mState)).read(mRecursiveBuffer[0], PredSender::Signature::variadicIndex);
                    }

                    mVariadicInfo.mRecursionLevel = 0;
                    if (!mVariadicInfo.mStates.empty()) {
                        mVariadicInfo.mStates.front()->start();
                    } else {
                        mState->start();
                    }
                } else {
                    mState->start();
                }
            }

            void buildState(InnerReceiver receiver, value_argument_tuple&& args)
            {
                construct(mState,
                    DelayedConstruct<State> { [&]() { return Execution::connect(buildSender(std::move(args), &mResults, &mRecursiveBuffer), std::forward<InnerReceiver>(receiver)); } });

                if constexpr (isVariadic) {

                    size_t baseIndex = 0;
                    uint32_t group = 1;

                    using PredSender = typename argument_types::template filter<is_pred_sender>::template unpack_unique<Execution::pred_sender<Execution::signature<>>>;
                    if constexpr (PredSender::Signature::variadic) {
                        baseIndex = PredSender::Signature::variadicIndex + 1;
                        group = 0;
                    }

                    mVariadicInfo.mStates.resize(Execution::get_context(receiver).mNode.variadicPinCount(group), std::nullopt);
                    for (auto& state : mVariadicInfo.mStates) {
                        construct(state, Execution::connect(buildVariadicSender(&baseIndex, &mRecursiveBuffer), RecursiveReceiver { this }));
                    }
                }
            }

            void cleanup()
            {
                if constexpr (isVariadic) {
                    while (mVariadicInfo.mRecursionLevel < mVariadicInfo.mStates.size()) {
                        auto& state = mVariadicInfo.mStates[mVariadicInfo.mRecursionLevel];
                        destruct(state);
                    }
                    mVariadicInfo.mStates.clear();
                }
                destruct(mState);
            }

            void continueRecursion()
            {
                if constexpr (!Traits::constant) {
                    auto& state = mVariadicInfo.mStates[mVariadicInfo.mRecursionLevel];
                    destruct(state);
                }
                ++mVariadicInfo.mRecursionLevel;
                if (mVariadicInfo.mRecursionLevel < mVariadicInfo.mStates.size()) {
                    mVariadicInfo.mStates[mVariadicInfo.mRecursionLevel]->start();
                } else {
                    mState->start();
                }
            }

            template <fixed_string Name>
            std::string_view getDynamicName()
            {
                return Execution::get_context(Execution::get_receiver(mState)).mNode.template getDynamicName<Name>();
            }

            virtual bool readVar(ValueType& result, std::string_view name) override
            {
                if constexpr (hasVariables) {
                    return [&]<fixed_string... Names, typename... T>(type_pack<Execution::variable<Names, T>...>)
                    {
                        return ([&]() {                    
                            if (name == getDynamicName<Names>()) {
                                return Execution::resolve_var<Names>(mState, result);
                            }
                            return false; }() || ...);
                    }
                    (typename Traits::exposedVariables {});
                }
                return false;
            }

            virtual std::vector<std::string_view> variables() override
            {
                if constexpr (hasVariables) {
                    return [&]<fixed_string... Names, typename... T>(type_pack<Execution::variable<Names, T>...>)
                    {
                        return std::vector<std::string_view> { Names... };
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
            ManualLifetime<State> mState = std::nullopt;

            static auto variadicHelper()
            {
                if constexpr (isVariadic) {
                    struct VariadicInfo {
                        using State = Execution::connect_result_t<VariadicSender, RecursiveReceiver>;
                        std::vector<ManualLifetime<State>> mStates;
                        size_t mRecursionLevel;
                    };
                    return VariadicInfo {};
                } else {
                    return std::monostate {};
                }
            }

            decltype(variadicHelper()) mVariadicInfo;
        };

        void setupInterpret(NodeInterpreterStateBase& interpreter, std::unique_ptr<NodeInterpreterData>& data) const override
        {
            if constexpr (hasVariables) {
                data = std::make_unique<InterpretData>(DummyReceiver { interpreter, *this }, mArguments);
            }
        }

        void interpret(NodeReceiver<NodeBase> receiver, std::unique_ptr<NodeInterpreterData>& data, uint32_t flowIn, uint32_t group) const override
        {
            if constexpr (!Traits::constant) {
                if (!data) {
                    data = std::make_unique<InterpretData>();
                }
                static_cast<InterpretData*>(data.get())->start({ receiver.mInterpreter, static_cast<const SenderNode<Algorithm>&>(receiver.mNode), receiver.mReceiver }, mArguments);
            } else {
                throw 0;
            }
        }

        void interpretRead(NodeInterpreterStateBase& interpreter, ValueType& retVal, std::unique_ptr<NodeInterpreterData>& data, uint32_t providerIndex, uint32_t group) const override
        {
            if constexpr (Traits::constant) {
                if constexpr (hasVariables) {
                    throw 0; //TODO
                } else {
                    DummyReceiver rec { interpreter, *this };

                    InterpretData data { rec, mArguments };

                    data.start();

                    assert(rec.mHasReturned);
                    retVal = rec.mValues[providerIndex];
                }
            } else {
                assert(data);
                retVal = static_cast<InterpretData*>(data.get())->read(providerIndex, group);
            }
        }

        virtual CodeGen::Statement generateRead(CodeGenerator& generator, std::unique_ptr<CodeGeneratorData>& data, uint32_t providerIndex, uint32_t group) const override
        {

            auto result = CodeGen::generatorFromSender(buildSender(value_argument_tuple { mArguments }), NodeCodegenReceiver { 0, this, generator }).generate();
            if constexpr (std::tuple_size_v<decltype(result)> < 1)
                throw 0;
            else {
                CodeGen::Statement current = TupleUnpacker::select(
                    TupleUnpacker::ensureTuple(std::get<0>(result)),
                    [](auto&& val) { return CodeGen::Statement { std::forward<decltype(val)>(val) }; },
                    providerIndex);

                if constexpr (isVariadic) {
                    for (size_t i = 0; i < this->variadicPinCount(group); ++i) {
                        auto result = CodeGen::generatorFromSender(buildVariadicSender(nullptr), NodeCodegenReceiver { 0, this, generator }).generate();
                        if constexpr (std::tuple_size_v<decltype(result)> >= 1)
                            current = std::get<0>(result);
                        else
                            throw 0;
                    }
                }

                return current;
            }
        }

        value_argument_tuple mArguments;
        template <size_t I>
        const decayed_t<std::tuple_element_t<I, value_argument_tuple>>& getArguments() const
        {
            return std::get<I>(mArguments);
        }
        template <size_t I>
        void setArguments(decayed_t<std::tuple_element_t<I, value_argument_tuple>> v)
        {
            std::get<I>(mArguments) = v;
        }

        template <fixed_string Name>
        struct NamedString {
            std::string mString;
        };

        static auto namesHelper()
        {
            if constexpr (requires { typename Traits::dynamicVariableNames; }) {
                return []<fixed_string... Names>(type_pack<auto_holder<Names>...>)
                {
                    return std::make_tuple(NamedString<Names> { std::string { Names } }...);
                }
                (typename Traits::dynamicVariableNames {});
            } else {
                return std::monostate {};
            }
        }
        decltype(namesHelper()) mDynamicNames = namesHelper();
        template <fixed_string Name>
        std::string_view getDynamicName() const
        {
            return std::get<NamedString<Name>>(mDynamicNames).mString;
        }
        template <fixed_string Name>
        void setDynamicName(std::string_view name)
        {
            std::get<NamedString<Name>>(mDynamicNames).mString = std::string { name };
        }
    };
}
}