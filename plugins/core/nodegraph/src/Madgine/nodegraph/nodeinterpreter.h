#pragma once

#include "Meta/keyvalue/valuetype.h"

#include "Generic/execution/virtualsender.h"

#include "Generic/genericresult.h"

namespace Engine {
namespace NodeGraph {

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;

        virtual bool resolveVar(OutRef<ValueType> &ref, std::string_view name) { return false; }
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreter : Execution::VirtualReceiverBase<GenericResult> {

        NodeInterpreter(const NodeGraph *graph, const ArgumentList &args);
        NodeInterpreter(const NodeInterpreter &) = delete;
        NodeInterpreter(NodeInterpreter &&) = default;
        virtual ~NodeInterpreter() = default;

        NodeInterpreter &operator=(const NodeInterpreter &) = delete;

        void start();
        void interpret(Pin pin);

        ASYNC_STUB(interpretSubGraph, branch, Execution::make_simple_virtual_sender<GenericResult>);

        void branch(Execution::VirtualReceiverBase<GenericResult> &receiver, uint32_t flow);
        void branch(Execution::VirtualReceiverBase<GenericResult> &receiver, Pin pin);

        /* void read(ValueType &retVal, uint32_t dataInIndex);
        void write(uint32_t dataOutIndex, const ValueType &v);*/

        void setGraph(const NodeGraph *graph);
        const NodeGraph *graph() const;

        std::unique_ptr<NodeInterpreterData> &data(uint32_t index);

        bool resolveVar(OutRef<ValueType> &result, std::string_view name);

    //private:
        const NodeGraph *mGraph = nullptr;
        const NodeBase *mCurrentNode = nullptr;

        ArgumentList mArguments;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;
        IndexType<uint32_t> mGraphGeneration;
    };

}
}