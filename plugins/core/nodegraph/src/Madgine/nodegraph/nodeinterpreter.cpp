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

    NodeInterpreter::NodeInterpreter(const NodeGraph *graph, const ArgumentList &args)
        : mGraph(graph)
        , mArguments(args)
    {
    }

    void NodeInterpreter::interpret(Execution::VirtualReceiverBase<GenericResult> *receiver, uint32_t flowIn)
    {
        interpret(receiver, mGraph->mFlowOutPins[flowIn].mTarget);
    }

    void NodeInterpreter::interpret(Execution::VirtualReceiverBase<GenericResult> *receiver, Pin pin)
    {
        assert(mGraph);

        if (mGraphGeneration != mGraph->generation()) {
            mData.clear();
            mData.resize(mGraph->nodes().size());

            for (size_t i = 0; i < mData.size(); ++i) {
                mGraph->node(i + 1)->setupInterpret(*this, mData[i]);
            }

            mGraphGeneration = mGraph->generation();
        }

        Debug::Debugger::getSingleton().stepInto(receiver, std::make_unique<NodeDebugLocation>(nullptr, this));
        branch(*receiver, pin);
    }

    void NodeInterpreter::branch(Execution::VirtualReceiverBase<GenericResult> &receiver, Pin pin)
    {
        const NodeBase *node = nullptr;
        if (pin && pin.mNode) {
            node = mGraph->node(pin.mNode);
        }
        static_cast<NodeDebugLocation *>(Debug::Debugger::getSingleton().getLocation(&receiver))->mNode = node;

        Execution::detach(Debug::Debugger::getSingleton().yield(&receiver)
            | Execution::then([=, &receiver]() {
                  if (pin && pin.mNode) {
                      node->interpret({ node, *this, receiver }, mData[pin.mNode - 1], pin.mIndex, pin.mGroup);
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

    void NodeInterpreter::setGraph(const NodeGraph *graph)
    {
        if (mGraph != graph) {
            mGraph = graph;

            mData.clear();
            if (mGraph) {
                mData.resize(mGraph->nodes().size());
                mGraphGeneration = mGraph->generation();
            }
        }
    }

    const NodeGraph *NodeInterpreter::graph() const
    {
        return mGraph;
    }

    std::unique_ptr<NodeInterpreterData> &NodeInterpreter::data(uint32_t index)
    {
        return mData[index - 1];
    }

    bool NodeInterpreter::resolveVar(ValueType &result, std::string_view name)
    {
        bool gotValue = false;
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                bool hadValue = gotValue;
                gotValue = data->resolveVar(result, name);
                assert(!hadValue || !gotValue);
            }
        }
        return gotValue;
    }

    std::map<std::string_view, ValueType> NodeInterpreter::variables()
    {
        std::map<std::string_view, ValueType> variables;
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                std::ranges::move(data->variables(), std::inserter(variables, variables.end()));
            }
        }
        return variables;
    }
}
}