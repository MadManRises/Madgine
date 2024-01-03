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
    using is_algorithm = is_instance_auto1<decayed_t<T>, NodeAlgorithm>;

    template <typename T>
    using is_pred_sender = std::bool_constant<is_instance<decayed_t<T>, NodeReader>::value || is_instance<decayed_t<T>, NodeStream>::value>;

    template <typename T>
    struct is_succ_sender : is_instance_auto1<decayed_t<T>, NodeSender> {
    };

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
        using type = /*dynamic_value_type<I>*/ ValueType;
    };

    template <typename T, size_t I>
    using remove_deductors = typename remove_deductors_impl<T, I>::type;

    template <typename Config, auto Algorithm, typename... Arguments>
    struct SenderNode : Node<SenderNode<Config, Algorithm, Arguments...>, AutoMaskNode<>> {

        using argument_types = type_pack<Arguments...>;
        using algorithms = typename argument_types::template filter<is_algorithm>;
        using value_arguments = typename argument_types::template filter<is_value>;
        using value_argument_tuple = typename value_arguments::template transform_with_index<remove_deductors>::template instantiate<std::tuple>;
        using succ_senders = typename argument_types::template filter<is_succ_sender>;
        using in_types = typename argument_types::template filter<is_pred_sender>;

        static constexpr size_t variadicSuccCount = succ_senders::filter<Execution::is_stream>::size;

        static_assert(Config::constant || Config::exposedVariables::size == 0);

        template <uint32_t I>
        static auto buildArgs(const std::tuple<>& values, type_pack<> args, std::vector<NodeResults>* results = nullptr)
        {
            return std::make_tuple();
        }

        template <uint32_t I, typename... Vs, typename T, typename... Ts>
        static auto buildArgs(std::tuple<Vs...>&& values, type_pack<T, Ts...> args, std::vector<NodeResults>* results = nullptr)
        {
            if constexpr (is_pred_sender<T>::value) {
                return std::tuple_cat(
                    std::make_tuple(T {}),
                    buildArgs<I>(std::move(values), type_pack<Ts...> {}, results));
            } else if constexpr (is_succ_sender<T>::value) {
                return std::tuple_cat(
                    std::make_tuple(NodeSender<I + 1> {}),
                    buildArgs<I + 1>(std::move(values), type_pack<Ts...> {}, results));
            } else if constexpr (is_algorithm<T>::value) {
                assert(results);
                return std::tuple_cat(
                    std::make_tuple(NodeAlgorithm<I + 1> { *results }),
                    buildArgs<I + 1>(std::move(values), type_pack<Ts...> {}, results));
            } else {
                return TupleUnpacker::prepend<decayed_t<first_t<Vs...>>>(
                    std::get<0>(std::move(values)),
                    buildArgs<I>(TupleUnpacker::popFront(std::move(values)), type_pack<Ts...> {}, results));
            }
        }

        static auto buildSender(value_argument_tuple&& values, std::vector<NodeResults>* results = nullptr)
        {
            return TupleUnpacker::invokeFromTuple(Algorithm, buildArgs<0>(std::move(values), argument_types {}, results));
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

        template <typename Signature>
        ExtendedValueTypeDesc signature_type(uint32_t index) const
        {
            return [ this, index ]<typename... T>(type_pack<T...>)
            {
                ExtendedValueTypeDesc types[] = {
                    resolveType<T>()...
                };
                return types[index];
            }
            (Signature {});
        }

        template <typename Signature>
        ExtendedValueTypeDesc stream_type(uint32_t index) const
        {
            return [ this, index ]<typename... T>(type_pack<T...>)
            {
                ExtendedValueTypeDesc types[] = {
                    resolveType<T>()...
                };
                return types[index % Signature::size];
            }
            (Signature {});
        }

        using Sender = decltype(buildSender(std::declval<value_argument_tuple>()));

        struct DummyReceiver : NodeExecutionReceiver<SenderNode<Config, Algorithm, Arguments...>> {
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

            void set_done()
            {
                throw 0;
            }

            bool mHasReturned = false;
            NodeResults mValues;

            template <typename CPO, typename... Args>
            friend decltype(auto) tag_invoke(CPO f, DummyReceiver& receiver, Args&&... args)
            {
                return f(static_cast<NodeExecutionReceiver<SenderNode<Config, Algorithm, Arguments...>>&>(receiver), std::forward<Args>(args)...);
            }
        };

        SenderNode(NodeGraph& graph)
            : Node<SenderNode<Config, Algorithm, Arguments...>, AutoMaskNode<>>(graph)
        {
            this->setup();
        }

        SenderNode(const SenderNode& other, NodeGraph& graph)
            : Node<SenderNode<Config, Algorithm, Arguments...>, AutoMaskNode<>>(other, graph)
            , mArguments(other.mArguments)
            , mDynamicNames(other.mDynamicNames)
        {
        }

        size_t flowInCount(uint32_t group) const override
        {
            return !Config::constant;
        }

        std::string_view flowInName(uint32_t index, uint32_t group) const override
        {
            return "in";
        }

        size_t flowOutGroupCount() const override
        {
            constexpr size_t algorithm_count = argument_types::template filter<is_algorithm>::size;
            constexpr size_t succ_sender_count = succ_senders::size;

            return 1 + algorithm_count + succ_sender_count;
        }

        size_t flowOutBaseCount(uint32_t group) const override
        {
            static constexpr auto counts = []<typename... InnerAlg, typename... SuccSender>(type_pack<InnerAlg...>, type_pack<SuccSender...>)
            {
                return std::array {
                    static_cast<int>(!Config::constant),
                    ((void)sizeof(type_pack<InnerAlg>), 1)...,
                    ((void)sizeof(type_pack<SuccSender>), 1)...
                };
            }
            (algorithms {}, succ_senders {});
            return counts[group];
        }

        std::string_view flowOutName(uint32_t index, uint32_t group) const override
        {
            return "out";
        }

        bool flowOutVariadic(uint32_t group = 0) const override
        {
            return false;
        }

        size_t dataInGroupCount() const override
        {
            return in_types::size;
        }

        size_t dataInBaseCount(uint32_t group) const override
        {
            static constexpr auto sizes = []<typename... Inner>(type_pack<Inner...>)
            {
                return std::array<size_t, in_types::size> {
                    []() {
                        if constexpr (InstanceOf<Inner, NodeReader>) {
                            return Inner::Signature::size;
                        } else {
                            return 0;
                        }                        
                    }()...
                };
            }
            (in_types {});

            return sizes[group];
        }

        ExtendedValueTypeDesc dataInType(uint32_t index, uint32_t group, bool bidir = true) const override
        {
            static constexpr auto types = []<typename... Inner>(type_pack<Inner...>)
            {
                return std::array<ExtendedValueTypeDesc (SenderNode::*)(uint32_t) const, in_types::size> {
                    []() {
                        if constexpr (InstanceOf<Inner, NodeReader>) {
                            return &SenderNode::signature_type<typename Inner::Signature>;
                        } else {
                            return &SenderNode::stream_type<typename Inner::Signature>;
                        }
                    }()...
                };
            }
            (in_types {});
            return (this->*types[group])(index);
        }

        bool dataInVariadic(uint32_t group = 0) const override
        {
            static constexpr auto variadic = []<typename... Inner>(type_pack<Inner...>)
            {
                return std::array<bool, in_types::size> {
                    InstanceOf<Inner, NodeStream>...
                };
            }
            (in_types {});

            return variadic[group];
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
                    InnerAlg::Signature::size...
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
                    &SenderNode<Config, Algorithm, Arguments...>::signature_type<typename Sender::template value_types<Execution::signature>>,
                    &SenderNode<Config, Algorithm, Arguments...>::signature_type<typename InnerAlg::Signature>...
                };
            }
            (algorithms {});
            return (this->*types[group])(index);
        }

        bool resolveVariableType(ExtendedValueTypeDesc& type, std::string_view name) const override
        {
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
            (typename Config::exposedVariables {});
        }

        struct InterpretData : NodeInterpreterData {

            struct Receiver;

            using InnerReceiver = std::conditional_t<Config::constant,
                std::conditional_t<(Config::exposedVariables::size > 0),
                    DummyReceiver,
                    DummyReceiver&>,
                Receiver>;
            using State = Execution::connect_result_t<Sender, InnerReceiver>;

            InterpretData()
            {
                static_assert(!Config::constant);
            }

            InterpretData(InnerReceiver receiver, value_argument_tuple args)
            {
                static_assert(Config::constant);
                buildState(std::forward<InnerReceiver>(receiver), std::move(args));
            }

            ~InterpretData()
            {
                if constexpr (Config::constant) {
                    cleanup();
                }
            }

            struct Receiver : NodeReceiver<SenderNode<Config, Algorithm, Arguments...>> {
                InterpretData* mData;

                template <typename... Args>
                void set_value(Args&&... args)
                {
                    if (mData->mResults.empty())
                        mData->mResults.emplace_back();
                    mData->mResults.front() = { ValueType { std::forward<Args>(args) }... };
                    mData->cleanup();
                    NodeReceiver<SenderNode<Config, Algorithm, Arguments...>>::set_value();
                }

                void set_done()
                {
                    mData->cleanup();
                    NodeReceiver<SenderNode<Config, Algorithm, Arguments...>>::set_done();
                }

                void set_error(GenericResult result)
                {
                    mData->cleanup();
                    NodeReceiver<SenderNode<Config, Algorithm, Arguments...>>::set_error(result);
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
                    if constexpr (!constant) {
                        mData->cleanup();
                    }
                    throw 0;
                }

                void set_error(GenericResult result)
                {
                    if constexpr (!constant) {
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

            void start(NodeReceiver<SenderNode<Config, Algorithm, Arguments...>> receiver, value_argument_tuple args)
            {
                static_assert(!Config::constant);
                buildState(Receiver { receiver, this }, std::move(args));
                start();
            }

            void start()
            {
                mState->start();
            }

            void buildState(InnerReceiver receiver, value_argument_tuple&& args)
            {
                construct(mState,
                    DelayedConstruct<State> { [&]() { return Execution::connect(buildSender(std::move(args), &mResults), std::forward<InnerReceiver>(receiver)); } });
            }

            void cleanup()
            {
                destruct(mState);
            }

            template <fixed_string Name>
            std::string_view getDynamicName()
            {
                return Execution::get_context(Execution::get_receiver(mState)).mNode.template getDynamicName<Name>();
            }

            virtual bool readVar(ValueType& result, std::string_view name) override
            {
                return [&]<fixed_string... Names, typename... T>(type_pack<Execution::variable<Names, T>...>)
                {
                    return ([&]() {                    
                            if (name == getDynamicName<Names>()) {
                                return Execution::resolve_var<Names>(Execution::get_receiver(mState), result);
                            }
                            return false; }() || ...);
                }
                (typename Config::exposedVariables {});
            }

            virtual bool writeVar(std::string_view name, const ValueType& value) override
            {
                return [&]<fixed_string... Names, typename... T>(type_pack<Execution::variable<Names, T>...>)
                {
                    return ([&]() {                    
                            if (name == getDynamicName<Names>()) {
                                return Execution::store_var<Names>(Execution::get_receiver(mState), value);
                            }
                            return false; }() || ...);
                }
                (typename Config::exposedVariables {});
            }

            virtual std::vector<std::string_view> variables() override
            {
                return [&]<fixed_string... Names, typename... T>(type_pack<Execution::variable<Names, T>...>)
                {
                    return std::vector<std::string_view> { Names... };
                }
                (typename Config::exposedVariables {});
            }

            virtual void visitState(CallableView<void(const Execution::StateDescriptor&)> visitor) override
            {
                Execution::visit_state(mState, visitor);
            }

            ValueType read(uint32_t providerIndex, uint32_t group) const
            {
                return mResults[group][providerIndex];
            }

            std::vector<NodeResults> mResults;
            ManualLifetime<State> mState = std::nullopt;
        };

        void setupInterpret(NodeInterpreterStateBase& interpreter, std::unique_ptr<NodeInterpreterData>& data) const override
        {
            if constexpr (Config::exposedVariables::size > 0) {
                data = std::make_unique<InterpretData>(DummyReceiver { interpreter, *this }, mArguments);
            }
        }

        void interpret(NodeReceiver<NodeBase> receiver, std::unique_ptr<NodeInterpreterData>& data, uint32_t flowIn, uint32_t group) const override
        {
            if constexpr (!Config::constant) {
                if (!data) {
                    data = std::make_unique<InterpretData>();
                }
                static_cast<InterpretData*>(data.get())->start({ receiver.mInterpreter, static_cast<const SenderNode<Config, Algorithm, Arguments...>&>(receiver.mNode), receiver.mReceiver }, mArguments);
            } else {
                throw 0;
            }
        }

        void interpretRead(NodeInterpreterStateBase& interpreter, ValueType& retVal, std::unique_ptr<NodeInterpreterData>& data, uint32_t providerIndex, uint32_t group) const override
        {
            if constexpr (Config::constant) {
                if constexpr (Config::exposedVariables::size > 0) {
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
            return []<fixed_string... Names>(type_pack<auto_holder<Names>...>)
            {
                return std::make_tuple(NamedString<Names> { std::string { Names } }...);
            }
            (typename Config::dynamicVariableNames {});
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