#pragma once

#include "Generic/genericresult.h"

#include "Meta/keyvalue/keyvaluereceiver.h"

#include "Madgine/behavior.h"
#include "Madgine/behaviorcollector.h"

#include "nodegraphloader.h"

namespace Engine {
namespace NodeGraph {

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;

        virtual bool resolveVar(ValueType &ref, std::string_view name) { return false; }
        virtual std::map<std::string_view, ValueType> variables() { return {}; }
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreterState : BehaviorState<NodeInterpreterState>, VariableScope {
        NodeInterpreterState() = default;
        NodeInterpreterState(VariableScope *parent);
        NodeInterpreterState(const NodeGraph *graph, const ArgumentList &args = {}, VariableScope *parent = nullptr);
        NodeInterpreterState(NodeGraphLoader::Resource *graph, const ArgumentList &args = {}, VariableScope *parent = nullptr);
        NodeInterpreterState(std::string_view name, const ArgumentList &args = {}, VariableScope *parent = nullptr);
        NodeInterpreterState(const NodeInterpreterState &) = delete;
        NodeInterpreterState(NodeInterpreterState &&) = default;
        virtual ~NodeInterpreterState() = default;

        NodeInterpreterState &operator=(const NodeInterpreterState &) = delete;
        NodeInterpreterState &operator=(NodeInterpreterState &&) = default;

        void interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, uint32_t flowIn) override;
        void interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin);

        ASYNC_STUB(interpret, interpretImpl, Execution::make_simple_virtual_sender<InterpretResult>);
        ASYNC_STUB(interpretSubGraph, branch, Execution::make_simple_virtual_sender<InterpretResult>);

        void branch(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin);

        void read(ValueType &retVal, Pin pin);
        void write(Pin pin, const ValueType &v);

        void read(ValueType &retVal, uint32_t dataProvider);
        void write(uint32_t dataReceiver, const ValueType &v);

        void setGraph(const NodeGraph *graph);
        const NodeGraph *graph() const;

        void setArguments(const ArgumentList &args);
        ArgumentList &arguments();

        std::unique_ptr<NodeInterpreterData> &data(uint32_t index);

        bool resolveVar(ValueType &result, std::string_view name, bool recursive = true) override;
        std::map<std::string_view, ValueType> variables() override;


        std::string_view graphName() const;

        void set(NodeGraphLoader::Resource *resource);
        NodeGraphLoader::Resource *get() const;

    private:
        const NodeGraph *mGraph = nullptr;

        ArgumentList mArguments;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;

        VariableScope *mParentScope = nullptr;

        NodeGraphLoader::Handle mGraphHandle;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::NodeInterpreterState);