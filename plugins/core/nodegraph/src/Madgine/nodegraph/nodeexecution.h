#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/state.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpretHandle {
        const NodeBase *mNode;
        NodeInterpreter &mInterpreter;

        void read(ValueType &retVal, uint32_t dataInIndex, uint32_t group = 0);

        void write(const ValueType &v, uint32_t dataOutIndex, uint32_t group = 0);

        ValueType resolveVar(std::string_view name);

        template <fixed_string Name>
        friend ValueType tag_invoke(Execution::resolve_var_t<Name>, NodeInterpretHandle &handle)
        {
            return handle.resolveVar(Name);
        }
    };

    using NodeExecutionReceiver = Execution::execution_receiver<NodeInterpretHandle>;

    template <uint32_t flowOutIndex, typename _Rec>
    struct NodeState {
        using Rec = _Rec;

        void start()
        {
            NodeInterpretHandle &handle = Execution::get_context(mRec);
            Execution::connect(handle.mInterpreter.interpretSubGraph(handle.mNode->flowOutTarget(0, flowOutIndex)), std::forward<Rec>(mRec)).start();
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
        using value_types = Tuple<decayed_t<T>...>;

        NodeReader(size_t *baseIndex = nullptr, NodeResults *variadicBuffer = nullptr)
            : mBaseIndex(baseIndex ? *baseIndex : 0)
            , mVariadicBuffer(variadicBuffer)
        {
            if (baseIndex)
                *baseIndex += sizeof...(T);
        }

        template <typename Rec>
        struct state : Execution::base_state<Rec> {

            template <typename T>
            struct typed_Value : ValueType {
                using ValueType::operator=;
            };
            void start()
            {
                helper(std::index_sequence_for<T...> {});
            }
            template <size_t... I>
            void helper(std::index_sequence<I...>)
            {
                size_t variadicIndex = 0;
                NodeInterpretHandle &handle = Execution::get_context(mRec);
                std::tuple<typed_Value<T>...> data;
                TupleUnpacker::forEach(data, [&]<typename T>(typed_Value<T> &v) {
                    if constexpr (InstanceOf<T, Execution::recursive>) {
                        assert(mVariadicBuffer);
                        v = (*mVariadicBuffer)[variadicIndex++];
                    } else {
                        handle.read(v, mIndex++);
                    }
                });
                set_value(std::get<I>(data).as<decayed_t<T>>()...);
            }

            size_t mIndex = 0;
            NodeResults *mVariadicBuffer = nullptr;
        };

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, NodeReader &&reader, Rec &&rec)
        {
            return state<Rec> { std::forward<Rec>(rec), reader.mBaseIndex, reader.mVariadicBuffer };
        }

        size_t mBaseIndex = 0;
        NodeResults *mVariadicBuffer = nullptr;
    };

    template <uint32_t flowOutIndex>
    struct NodeAlgorithm {
        template <typename... Args>
        auto operator()(Args &&...args)
        {
            if (mResults.size() <= flowOutIndex)
                mResults.resize(flowOutIndex + 1);
            mResults[flowOutIndex] = { ValueType { std::forward<Args>(args) }... };
            return NodeSender<flowOutIndex> {};
        }
        std::vector<NodeResults> &mResults;
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeReceiver : NodeExecutionReceiver {
        Execution::VirtualReceiverBase<GenericResult> &mReceiver;

        void set_value();
        void set_done();
        void set_error(GenericResult result);
    };

}
}