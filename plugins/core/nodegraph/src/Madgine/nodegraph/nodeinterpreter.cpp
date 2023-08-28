#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "nodegraph.h"

#include "Meta/keyvalue/valuetype.h"

#include "nodeexecution.h"

#include "Madgine/debug/debugger.h"

namespace Engine {
namespace NodeGraph {

    struct NodeDebugLocation : Debug::DebugLocation {
        NodeDebugLocation(const NodeBase *node, NodeInterpreter *interpreter)
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
        NodeInterpreter *mInterpreter;
    };

    NodeInterpreter::NodeInterpreter(const NodeGraph *graph, const ArgumentList &args, NodeVariableScope *parent)
        : mArguments(args)
        , mParentScope(parent)
    {
        setGraph(graph);
    }

    void NodeInterpreter::interpretImpl(Execution::VirtualReceiverBase<NodeInterpretResult> &receiver, uint32_t flowIn)
    {
        interpretImpl(receiver, mGraph->mFlowOutPins[flowIn].mTarget);
    }

    void NodeInterpreter::interpretImpl(Execution::VirtualReceiverBase<NodeInterpretResult> &receiver, Pin pin)
    {
        Debug::Debugger::getSingleton().stepInto(&receiver, std::make_unique<NodeDebugLocation>(nullptr, this));
        branch(receiver, pin);
    }

    void NodeInterpreter::branch(Execution::VirtualReceiverBase<NodeInterpretResult> &receiver, Pin pin)
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

    void NodeInterpreter::read(ValueType &retVal, Pin pin)
    {
        if (!pin) {
            throw 0;
        } else if (!pin.mNode) {
            retVal = mArguments.at(pin.mIndex);
        } else {
            mGraph->node(pin.mNode)->interpretRead(*this, retVal, mData[pin.mNode - 1], pin.mIndex, pin.mGroup);
        }
    }

    void NodeInterpreter::write(Pin pin, const ValueType &v)
    {
        mGraph->node(pin.mNode)->interpretWrite(*this, mData[pin.mNode - 1], v, pin.mIndex, pin.mGroup);
    }

    void NodeInterpreter::read(ValueType &retVal, uint32_t dataProvider)
    {
        read(retVal, mGraph->mDataInPins[dataProvider].mSource);
    }

    void NodeInterpreter::write(uint32_t dataReceiver, const ValueType &v)
    {
        write(mGraph->mDataOutPins[dataReceiver].mTarget, v);
    }

    void NodeInterpreter::setGraph(const NodeGraph *graph)
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

    const NodeGraph *NodeInterpreter::graph() const
    {
        return mGraph;
    }

    void NodeInterpreter::setArguments(const ArgumentList &args)
    {
        mArguments = args;
    }

    ArgumentList &NodeInterpreter::arguments()
    {
        return mArguments;
    }

    std::unique_ptr<NodeInterpreterData> &NodeInterpreter::data(uint32_t index)
    {
        return mData[index - 1];
    }

    bool NodeInterpreter::resolveVar(ValueType &result, std::string_view name, bool recursive)
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

    std::map<std::string_view, ValueType> NodeInterpreter::variables(bool recursive)
    {
        std::map<std::string_view, ValueType> variables;
        if (recursive && mParentScope)
            variables = mParentScope->variables();
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                std::ranges::move(data->variables(), std::inserter(variables, variables.end()));
            }
        }
        return variables;
    }
}
}