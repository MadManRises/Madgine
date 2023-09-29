#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "nodegraph.h"

#include "Meta/keyvalue/valuetype.h"

#include "nodeexecution.h"

#include "Madgine/debug/debugger.h"

#include "Meta/keyvalue/metatable_impl.h"

NAMED_UNIQUECOMPONENT(NodeGraph, Engine::NodeGraph::NodeInterpreterState);

METATABLE_BEGIN(Engine::NodeGraph::NodeInterpreterState)
PROPERTY(Graph, get, set)
READONLY_PROPERTY(Variables, variables)
METATABLE_END(Engine::NodeGraph::NodeInterpreterState)

namespace Engine {
namespace NodeGraph {

    struct NodeDebugLocation : Debug::DebugLocation {
        NodeDebugLocation(const NodeBase *node, NodeInterpreterState *interpreter)
            : mNode(node)
            , mInterpreter(interpreter)
        {
        }

        virtual std::string toString() const override
        {
            std::stringstream ss;
            if (mNode) {
                ss << mNode->className() << " [" << mNode << "]";
            } else {
                ss << "Graph [" << mInterpreter->graph() << "]";
            }
            return ss.str();
        }

        virtual std::map<std::string_view, ValueType> localVariables() const override
        {
            return mInterpreter->variables();
        }

        const NodeBase *mNode;
        NodeInterpreterState *mInterpreter;
    };

    NodeInterpreterState::NodeInterpreterState(VariableScope *parent)
        : mParentScope(parent)
    {
    }

    NodeInterpreterState::NodeInterpreterState(const NodeGraph *graph, const ArgumentList &args, VariableScope *parent)
        : mArguments(args)
        , mParentScope(parent)
    {
        setGraph(graph);
    }

    NodeInterpreterState::NodeInterpreterState(NodeGraphLoader::Resource *graph, const ArgumentList &args, VariableScope *parent)
        : mArguments(args)
        , mParentScope(parent)
        , mGraphHandle(graph)
    {
        mGraphHandle.info()->loadingTask().then([=](bool result) {if (result)
                setGraph(&*mGraphHandle); }, NodeGraphLoader::getSingleton().loadingTaskQueue());
    }

    NodeInterpreterState::NodeInterpreterState(std::string_view name, const ArgumentList &args, VariableScope *parent)
        : mArguments(args)
        , mParentScope(parent)
    {
        mGraphHandle.load(name).then([=](bool result) {if (result)
                setGraph(&*mGraphHandle); }, NodeGraphLoader::getSingleton().loadingTaskQueue());
    }

    void NodeInterpreterState::interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, uint32_t flowIn)
    {
        interpretImpl(receiver, mGraph->mFlowOutPins[flowIn].mTarget);
    }

    void NodeInterpreterState::interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin)
    {
        Debug::Debugger::getSingleton().stepInto(&receiver, std::make_unique<NodeDebugLocation>(nullptr, this));
        branch(receiver, pin);
    }

    void NodeInterpreterState::branch(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin)
    {

        const NodeBase *node = nullptr;
        if (pin && pin.mNode) {
            node = mGraph->node(pin.mNode);
        }
        NodeDebugLocation *location = static_cast<NodeDebugLocation *>(Debug::Debugger::getSingleton().getLocation(&receiver));
        if (location)
            location->mNode = node;

        Execution::detach(Debug::Debugger::getSingleton().yield(&receiver)
            | Execution::then([=, &receiver]() {
                  if (pin && pin.mNode) {
                      node->interpret({ *this, *node, receiver }, mData[pin.mNode - 1], pin.mIndex, pin.mGroup);
                  } else {
                      Debug::Debugger::getSingleton().stepOut(&receiver);
                      receiver.set_value();
                  }
              }));
    }

    void NodeInterpreterState::read(ValueType &retVal, Pin pin)
    {
        if (!pin) {
            throw 0;
        } else if (!pin.mNode) {
            retVal = mArguments.at(pin.mIndex);
        } else {
            mGraph->node(pin.mNode)->interpretRead(*this, retVal, mData[pin.mNode - 1], pin.mIndex, pin.mGroup);
        }
    }

    void NodeInterpreterState::write(Pin pin, const ValueType &v)
    {
        mGraph->node(pin.mNode)->interpretWrite(*this, mData[pin.mNode - 1], v, pin.mIndex, pin.mGroup);
    }

    void NodeInterpreterState::read(ValueType &retVal, uint32_t dataProvider)
    {
        read(retVal, mGraph->mDataInPins[dataProvider].mSource);
    }

    void NodeInterpreterState::write(uint32_t dataReceiver, const ValueType &v)
    {
        write(mGraph->mDataOutPins[dataReceiver].mTarget, v);
    }

    void NodeInterpreterState::setGraph(const NodeGraph *graph)
    {
        if (mGraph != graph) {
            mGraph = graph;

            mData.clear();
            if (mGraph) {
                mData.resize(mGraph->nodes().size());

                for (size_t i = 0; i < mData.size(); ++i) {
                    mGraph->node(i + 1)->setupInterpret(*this, mData[i]);
                }
            }
        }
    }

    const NodeGraph *NodeInterpreterState::graph() const
    {
        return mGraph;
    }

    void NodeInterpreterState::setArguments(const ArgumentList &args)
    {
        mArguments = args;
    }

    ArgumentList &NodeInterpreterState::arguments()
    {
        return mArguments;
    }

    std::unique_ptr<NodeInterpreterData> &NodeInterpreterState::data(uint32_t index)
    {
        return mData[index - 1];
    }

    bool NodeInterpreterState::resolveVar(ValueType &result, std::string_view name, bool recursive)
    {
        bool gotValue = false;
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                bool hadValue = gotValue;
                gotValue = data->resolveVar(result, name);
                assert(!hadValue || !gotValue);
            }
        }
        if (!gotValue && recursive && mParentScope)
            return mParentScope->resolveVar(result, name);
        return gotValue;
    }

    std::map<std::string_view, ValueType> NodeInterpreterState::variables()
    {
        std::map<std::string_view, ValueType> variables;
        if (false && mParentScope)
            variables = mParentScope->variables();
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                std::ranges::move(data->variables(), std::inserter(variables, variables.end()));
            }
        }
        return variables;
    }

    std::string_view NodeInterpreterState::graphName() const
    {
        return mGraphHandle.name();
    }

    void NodeInterpreterState::set(NodeGraphLoader::Resource *resource)
    {
        setGraph(nullptr);
        mGraphHandle = resource;
        mGraphHandle.info()->loadingTask().then([=](bool result) {if (result)
                setGraph(&*mGraphHandle); }, NodeGraphLoader::getSingleton().loadingTaskQueue());
    }

    NodeGraphLoader::Resource *NodeInterpreterState::get() const
    {
        return mGraphHandle.resource();
    }

}
}