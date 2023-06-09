#pragma once

#include "Generic/execution/virtualstate.h"

#include "Generic/genericresult.h"

#include "Meta/keyvalue/keyvaluereceiver.h"

namespace Engine {
namespace NodeGraph {

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;

        virtual bool resolveVar(ValueType &ref, std::string_view name) { return false; }
        virtual std::map<std::string_view, ValueType> variables() { return {}; }
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreter {

        NodeInterpreter(const NodeGraph *graph, const ArgumentList &args);
        NodeInterpreter(const NodeInterpreter &) = delete;
        NodeInterpreter(NodeInterpreter &&) = default;
        virtual ~NodeInterpreter() = default;

        NodeInterpreter &operator=(const NodeInterpreter &) = delete;

        void interpret(Execution::VirtualReceiverBase<GenericResult> *receiver, uint32_t flowIn);
        void interpret(Execution::VirtualReceiverBase<GenericResult> *receiver, Pin pin);

        ASYNC_STUB(interpretSubGraph, branch, Execution::make_simple_virtual_sender<GenericResult>);

        void branch(Execution::VirtualReceiverBase<GenericResult> &receiver, Pin pin);

        void read(ValueType &retVal, Pin pin);
        void write(Pin pin, const ValueType &v);

        void setGraph(const NodeGraph *graph);
        const NodeGraph *graph() const;

        std::unique_ptr<NodeInterpreterData> &data(uint32_t index);

        bool resolveVar(ValueType &result, std::string_view name);
        std::map<std::string_view, ValueType> variables();

    private:
        const NodeGraph *mGraph = nullptr;

        ArgumentList mArguments;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;
        IndexType<uint32_t> mGraphGeneration;
    };

}
}