#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "../../nodecollector.h"
#include "../../nodeinterpreter.h"
#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/state.h"

#include "../../pins.h"
#include "../../nodegraph.h"
#include "sendertraits.h"

namespace Engine {
namespace NodeGraph {

    template <typename T>
    using instance_helper = is_instance<T, algorithm>;

    template <typename T>
    using value_helper = std::negation<is_instance<T, algorithm>>;

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

    template <typename Signature, typename... Rest>
    struct algorithm_argument_helper<algorithm<Signature>, Rest...> {
        using base = algorithm_argument_helper<Rest...>;

        static constexpr size_t count = Signature::count + base::count;

        static ExtendedValueTypeDesc type(uint32_t index)
        {
            if (index < count) {
                return Signature::type(index);
            } else {
                return base::type(index - count);
            }
        }
    };

    template <typename Algorithm>
    struct SenderNode : Node<SenderNode<Algorithm>> {

        using Traits = sender_traits<Algorithm>;
        using Sender = typename Traits::Sender;

        using algorithm_arguments = typename Traits::argument_types::filter<instance_helper>::instantiate<algorithm_argument_helper>;
        using value_argument_tuple = typename Traits::argument_types::filter<value_helper>::instantiate<std::tuple>;

        SenderNode(NodeGraph &graph)
            : Node<SenderNode<Algorithm>>(graph)
        {
            setup();
        }

        SenderNode(const SenderNode &other, NodeGraph &graph)
            : Node<SenderNode<Algorithm>>(graph)
        {
        }

        size_t flowInCount() const override
        {
            return !Traits::constant;
        }

        std::string_view flowInName(uint32_t index) const override
        {
            return "in";
        }

        size_t flowOutCount() const override
        {
            constexpr size_t algorithm_count = Traits::argument_types::filter<instance_helper>::size;
            return !Traits::constant + algorithm_count;
        }

        std::string_view flowOutName(uint32_t index) const override
        {
            return "out";
        }

        size_t dataInCount() const override
        {
            return Traits::in_types::count;
        }

        ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override
        {
            return Traits::in_types::type(index);
        }

        size_t dataProviderCount() const override
        {
            return Sender::value_types<type_pack>::size + algorithm_arguments::count;
        }

        ExtendedValueTypeDesc dataProviderType(uint32_t index, bool bidir = true) const override
        {
            if (index >= Sender::value_types<type_pack>::size)
                return algorithm_arguments::type(index - Sender::value_types<type_pack>::size);
            else
                return toValueTypeDesc<ValueType>();
        }

        struct InterpretData : NodeInterpreterData {

            using NodeSender = typename Traits::Sender;

            InterpretData(NodeInterpretHandle handle, const value_argument_tuple &args)
            {
                if constexpr (Traits::constant) {
                    new (&mState) State { Execution::connect(TupleUnpacker::invokeExpand(&Traits::buildSender, mAlgorithmResults, args), NoopReceiver { handle }) };
                }
            }

            ~InterpretData()
            {
                if constexpr (Traits::constant) {
                    mState.~State();
                }
            }

            struct Receiver {
                NodeReceiver mReceiver;
                InterpretData *mData;

                template <typename... Args>
                void set_value(Args &&...args)
                {
                    mData->mResults = { ValueType { std::forward<Args>(args) }... };
                    mData->mState.~State();
                    mReceiver.set_value();
                }

                void set_done()
                {
                    mData->mState.~State();
                    mReceiver.set_done();
                }

                void set_error(GenericResult result)
                {
                    mData->mState.~State();
                    mReceiver.set_error(result);
                }

                template <typename CPO>
                friend decltype(auto) tag_invoke(CPO f, Receiver &receiver)
                {
                    return f(receiver.mReceiver);
                }
            };

            using State = Execution::connect_result_t<NodeSender, std::conditional_t<Traits::constant, NoopReceiver, Receiver>>;

            void start(NodeReceiver receiver, const value_argument_tuple &args)
            {
                if constexpr (!Traits::constant) {
                    new (&mState) State { Execution::connect(TupleUnpacker::invokeExpand(&Traits::buildSender, mAlgorithmResults, args), Receiver { receiver, this }) };

                    mState.start();
                } else {
                    throw 0;
                }
            }

            template <fixed_string... Names>
            bool resolveHelper(OutRef<ValueType> &result, std::string_view name, type_pack<auto_holder<Names>...>)
            {
                return ([&]() {                    
                    if (name == Names) {
                        result = Execution::resolve_var<Names>(mState);
                        return true;
                    }
                    return false; }() || ...);
            }

            virtual bool resolveVar(OutRef<ValueType> &result, std::string_view name) override
            {
                if constexpr (requires { typename Traits::exposedVariables; }) {
                    return resolveHelper(result, name, typename Traits::exposedVariables {});
                }
                return false;
            }

            NodeResults mResults;
            union {
                State mState;
            };
            NodeResults mAlgorithmResults;
        };

        void setupInterpret(NodeInterpreter &interpreter, std::unique_ptr<NodeInterpreterData> &data) const override
        {
            if constexpr (Traits::eagerNode) {
                data = std::make_unique<InterpretData>(NodeInterpretHandle { this, interpreter }, mArguments);
            }
        }

        void interpret(NodeReceiver receiver, uint32_t flowIn, std::unique_ptr<NodeInterpreterData> &data) const override
        {
            if (!data) {
                data = std::make_unique<InterpretData>(receiver, mArguments);
            }
            static_cast<InterpretData *>(data.get())->start(std::move(receiver), mArguments);
        }

        struct DummyReceiver : NodeInterpretHandle {
            template <typename... Args>
            void set_value(Args &&...args)
            {
                mValues = { ValueType { std::forward<Args>(args) }... };
                mHasReturned = true;
            }

            bool mHasReturned = false;
            NodeResults mValues;
        };

        void interpretRead(NodeInterpreter &interpreter, ValueType &retVal, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const override
        {
            if constexpr (Traits::constant) {

                DummyReceiver rec { this, interpreter };

                NodeResults algorithmResults;
                Execution::connect(TupleUnpacker::invokeExpand(&Traits::buildSender, algorithmResults, mArguments), rec).start();

                assert(rec.mHasReturned);
                retVal = rec.mValues[providerIndex];
            } else {
                assert(data);

                if (providerIndex >= Sender::value_types<type_pack>::size)
                    retVal = static_cast<InterpretData *>(data.get())->mAlgorithmResults[providerIndex - Sender::value_types<type_pack>::size];
                else
                    retVal = static_cast<InterpretData *>(data.get())->mResults[providerIndex];
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