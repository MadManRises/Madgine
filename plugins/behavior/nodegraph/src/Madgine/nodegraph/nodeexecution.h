#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/state.h"

#include "Generic/execution/algorithm.h"
#include "Generic/execution/execution.h"
#include "Generic/execution/sendertraits.h"
#include "Generic/execution/state.h"

#include "Meta/keyvalue/valuetype.h"

#include "nodebase.h"
#include "nodeinterpreter.h"

#include "Madgine/codegen/fromsender.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpretHandleBase {
        NodeInterpreterStateBase &mInterpreter;

        void read(const NodeBase &node, ValueType &retVal, uint32_t dataInIndex, uint32_t group = 0);

        void write(const NodeBase &node, const ValueType &v, uint32_t dataOutIndex, uint32_t group = 0);

        bool readVar(std::string_view name, ValueType &out);
        bool writeVar(std::string_view name, const ValueType &out);
    };

    template <typename Node>
    struct NodeInterpretHandle : NodeInterpretHandleBase {
        const Node &mNode;

        void read(ValueType &retVal, uint32_t dataInIndex, uint32_t group = 0)
        {
            NodeInterpretHandleBase::read(mNode, retVal, dataInIndex, group);
        }

        void write(const ValueType &v, uint32_t dataOutIndex, uint32_t group = 0)
        {
            NodeInterpretHandleBase::write(mNode, v, dataOutIndex, group);
        }

        template <fixed_string Name, typename O>
        friend bool tag_invoke(Execution::resolve_var_t<Name>, NodeInterpretHandle &handle, O &out)
        {
            ValueType v;
            if (handle.readVar(handle.mNode.template getDynamicName<Name>(), v)) {
                out = v.as<O>();
                return true;
            } else {
                return false;
            }
        }

        template <fixed_string Name, typename T>
        friend bool tag_invoke(Execution::store_var_t<Name>, NodeInterpretHandle &handle, T &&value)
        {
            return handle.writeVar(handle.mNode.template getDynamicName<Name>(), std::forward<T>(value));
        }

        friend Debug::DebugLocation *tag_invoke(Execution::get_debug_location_t, NodeInterpretHandle &handle)
        {
            return handle.mInterpreter.debugLocation();
        }
    };

    template <typename Node>
    using NodeExecutionReceiver = Execution::execution_receiver<NodeInterpretHandle<Node>>;

    struct MADGINE_NODEGRAPH_EXPORT NodeCodegenHandle : CodeGen::CodeGen_Context {
        const NodeBase *mNode;
        CodeGenerator &mGenerator;

        CodeGen::Statement read(uint32_t dataInIndex, uint32_t group = 0);
    };

    using NodeCodegenReceiver = Execution::execution_receiver<NodeCodegenHandle>;

    template <uint32_t flowOutIndex, typename _Rec>
    struct NodeState {
        using Rec = _Rec;

        void start()
        {
            auto &handle = Execution::get_context(mRec);
            Execution::connect(handle.mInterpreter.interpretSubGraph(handle.mNode.flowOutTarget(0, flowOutIndex)), std::forward<Rec>(mRec)).start();
        }

        friend Rec &tag_invoke(Execution::get_receiver_t, NodeState &state)
        {
            return state.mRec;
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

        template <typename Rec>
        friend auto tag_invoke(CodeGen::codegen_connect_t, NodeSender &&sender, Rec &&rec)
        {
            struct state : CodeGen::codegen_base_state<Rec> {
                auto generate()
                {
                    return std::make_tuple(CodeGen::Constant { ValueType { 0 } });
                }
            };
            return state { std::forward<Rec>(rec) };
        }
    };

    template <typename... T>
    struct NodeReader {
        using Signature = Execution::signature<T...>;

        using result_type = void;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<decayed_t<T>...>;

        NodeReader(size_t *baseIndex = nullptr)
            : mBaseIndex(baseIndex ? *baseIndex : 0)
        {
            if (baseIndex)
                *baseIndex += sizeof...(T);
        }

        template <typename Rec>
        struct state : Execution::base_state<Rec> {

            template <typename Ty>
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
                auto &handle = Execution::get_context(this->mRec);
                if (handle.mNode.dataInCount() == mIndex) {
                    this->set_done();
                } else {
                    std::tuple<typed_Value<T>...> data;
                    TupleUnpacker::forEach(data, [&]<typename Ty>(typed_Value<Ty> &v) {
                        handle.read(v, mIndex++);
                    });
                    this->set_value(std::get<I>(data).template as<decayed_t<T>>()...);
                }
            }

            size_t mIndex = 0;
        };

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, NodeReader &&reader, Rec &&rec)
        {
            return state<Rec> { std::forward<Rec>(rec), reader.mBaseIndex };
        }

        template <typename Rec>
        struct codegen_state : CodeGen::codegen_base_state<Rec> {
            auto generate()
            {
                return helper(std::index_sequence_for<T...> {});
            }
            template <size_t... I>
            auto helper(std::index_sequence<I...>)
            {
                return std::make_tuple(Execution::get_context(this->mRec).read(mIndex + I)...);
            }
            size_t mIndex;
        };

        template <typename Rec>
        friend auto tag_invoke(CodeGen::codegen_connect_t, NodeReader &&reader, Rec &&rec)
        {
            return codegen_state<Rec> { std::forward<Rec>(rec), reader.mBaseIndex };
        }

        size_t mBaseIndex = 0;
    };

    template <size_t flowOutIndex, typename... Arguments>
    struct NodeAlgorithm {

        using Signature = Execution::signature<Arguments...>;

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

    MADGINE_NODEGRAPH_EXPORT void continueExecution(NodeInterpreterStateBase &interpreter, const NodeBase &node, Execution::VirtualReceiverBase<BehaviorError> &receiver);

    template <typename Node>
    struct NodeReceiver : NodeExecutionReceiver<Node> {
        Execution::VirtualReceiverBase<BehaviorError> &mReceiver;

        void set_value()
        {
            continueExecution(this->mInterpreter, this->mNode, mReceiver);
        }
        void set_done()
        {
            mReceiver.set_done();
        }
        void set_error(BehaviorError result)
        {
            mReceiver.set_error(result);
        }
    };

    template <typename T>
    struct NodeStream {
        using Signature = Execution::signature<T>;

        NodeReader<T> next() {
            return { &mIndex };
        }

        size_t mIndex = 0;
    };

}
}