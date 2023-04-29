#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "nodecollector.h"
#include "nodeinterpreter.h"
#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/state.h"

namespace Engine {
namespace NodeGraph {

    struct NodeInterpretHandle {
        const NodeBase *mNode;
        NodeInterpreter &mInterpreter;

        void read(ValueType &retVal, uint32_t dataInIndex)
        {
            Pin pin = mNode->dataInSource(dataInIndex);
            if (!pin.mNode) {
                retVal = mInterpreter.mArguments.at(pin.mIndex);
            } else {
                mInterpreter.mGraph->node(pin.mNode)->interpretRead(mInterpreter, retVal, pin.mIndex, mInterpreter.mData[pin.mNode - 1]);
            }
        }

        void write(uint32_t dataOutIndex, const ValueType &v)
        {
            Pin pin = mNode->dataOutTarget(dataOutIndex);
            if (pin)
                mInterpreter.mGraph->node(pin.mNode)->interpretWrite(mInterpreter, pin.mIndex, mInterpreter.mData[pin.mNode - 1], v);
        }

        template <fixed_string Name>
        friend ValueType &tag_invoke(Execution::resolve_var_t<Name>, NodeInterpretHandle &handle)
        {
            OutRef<ValueType> v;
            bool result = handle.mInterpreter.resolveVar(v, Name);
            assert(result);
            return v;
        }

        friend NodeInterpretHandle &tag_invoke(Execution::get_context_t, NodeInterpretHandle &handle)
        {
            return handle;
        }
    };

    template <uint32_t flowOutIndex, typename _Rec>
    struct NodeState {
        using Rec = _Rec;

        void start()
        {
            NodeInterpretHandle &handle = Execution::get_context(mRec);
            Execution::connect(handle.mInterpreter.interpretSubGraph(handle.mNode->flowOutTarget(flowOutIndex)), std::forward<Rec>(mRec)).start();
        }

        Rec mRec;
    };

    template <uint32_t flowOutIndex>
    struct NodeSender {
        using result_type = void;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, NodeSender &&sender, Rec &&rec)
        {
            return NodeState<flowOutIndex, Rec> { std::forward<Rec>(rec) };
        }
    };

    template <typename... T>
    struct NodeReader {
        using result_type = void;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<T...>;

        template <typename Rec>
        struct state {
            using Rec = Rec;
            void start()
            {
                helper(std::index_sequence_for<T...> {});
            }
            template <size_t... I>
            void helper(std::index_sequence<I...>)
            {
                NodeInterpretHandle &handle = Execution::get_context(mRec);
                std::tuple<std::conditional_t<true, ValueType, T>...> data;
                size_t index = 0;
                TupleUnpacker::forEach(data, [&](ValueType &v) { handle.read(v, index++); });
                mRec.set_value(std::get<I>(data).as<T>()...);
            }

            Rec mRec;
        };

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, NodeReader &&sender, Rec &&rec)
        {
            return state<Rec> { std::forward<Rec>(rec) };
        }
    };

    template <uint32_t flowOutIndex>
    struct NodeAlgorithm {
        template <typename... Args>
        auto operator()(Args &&...args)
        {
            mArguments = { ValueType { std::forward<Args>(args) }... };
            return NodeSender<flowOutIndex> {};
        }
        NodeResults &mArguments;
    };

    struct NoopReceiver : NodeInterpretHandle {
    };

    struct NodeReceiver : NodeInterpretHandle {
        Execution::VirtualReceiverBase<GenericResult> &mReceiver;

        void set_value()
        {
            Pin pin = mNode->flowOutTarget(0);
            if (pin)
                mInterpreter.branch(mReceiver, pin);
            else
                mReceiver.set_value();
        }

        void set_done()
        {
            mReceiver.set_done();
        }

        void set_error(GenericResult result)
        {
            mReceiver.set_error(result);
        }
    };

}
}