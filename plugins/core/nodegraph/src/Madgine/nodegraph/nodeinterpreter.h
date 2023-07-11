#pragma once

#include "Generic/execution/virtualstate.h"

#include "Generic/genericresult.h"

#include "Meta/keyvalue/keyvaluereceiver.h"

namespace Engine {
namespace NodeGraph {

    ENUM_BASE(NodeInterpretResult, GenericResult);

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;

        virtual bool resolveVar(ValueType &ref, std::string_view name) { return false; }
        virtual std::map<std::string_view, ValueType> variables() { return {}; }
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreter {
        NodeInterpreter() = default;
        NodeInterpreter(const NodeGraph *graph, const ArgumentList &args);
        NodeInterpreter(const NodeInterpreter &) = delete;
        NodeInterpreter(NodeInterpreter &&) = default;
        virtual ~NodeInterpreter() = default;

        NodeInterpreter &operator=(const NodeInterpreter &) = delete;

        void interpretImpl(Execution::VirtualReceiverBase<NodeInterpretResult> &receiver, uint32_t flowIn);
        void interpretImpl(Execution::VirtualReceiverBase<NodeInterpretResult> &receiver, Pin pin);

        ASYNC_STUB(interpret, interpretImpl, Execution::make_simple_virtual_sender<NodeInterpretResult>);
        ASYNC_STUB(interpretSubGraph, branch, Execution::make_simple_virtual_sender<NodeInterpretResult>);

        void branch(Execution::VirtualReceiverBase<NodeInterpretResult> &receiver, Pin pin);

        void read(ValueType &retVal, Pin pin);
        void write(Pin pin, const ValueType &v);

        void read(ValueType &retVal, uint32_t dataProvider);
        void write(uint32_t dataReceiver, const ValueType &v);

        void setGraph(const NodeGraph *graph);
        const NodeGraph *graph() const;

        void setArguments(const ArgumentList &args);
        ArgumentList &arguments();

        std::unique_ptr<NodeInterpreterData> &data(uint32_t index);

        bool resolveVar(ValueType &result, std::string_view name);
        std::map<std::string_view, ValueType> variables();

    private:
        const NodeGraph *mGraph = nullptr;

        ArgumentList mArguments;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;
    };

}
}