#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "nodegraph.h"

#include "Meta/keyvalue/valuetype.h"

#include "nodeexecution.h"

#include "Madgine/debug/debugger.h"

namespace Engine {
namespace NodeGraph {

    struct NodeDebugLocation : Debug::DebugLocation {
        NodeDebugLocation(const NodeBase *node, NodeInterpreterStateBase *interpreter)
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
            std::map<std::string_view, ValueType> values;
            for (std::string_view name : mInterpreter->variables()) {
                mInterpreter->readVar(values.try_emplace(name).first->second, name);
            }
            return values;
        }

        const NodeBase *mNode;
        NodeInterpreterStateBase *mInterpreter;
    };

    NodeInterpreterStateBase::NodeInterpreterStateBase(const NodeGraph *graph)
        : mGraph(graph)
    {
        mData.resize(mGraph->nodes().size());

        for (size_t i = 0; i < mData.size(); ++i) {
            mGraph->node(i + 1)->setupInterpret(*this, mData[i]);
        }
    }

    void NodeInterpreterStateBase::interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, uint32_t flowIn)
    {
        interpretImpl(receiver, mGraph->mFlowOutPins[flowIn].mTarget);
    }

    void NodeInterpreterStateBase::interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin)
    {
        Debug::Debugger::getSingleton().stepInto(&receiver, std::make_unique<NodeDebugLocation>(nullptr, this));
        branch(receiver, pin);
    }

    void NodeInterpreterStateBase::branch(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin)
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

    void NodeInterpreterStateBase::read(ValueType &retVal, Pin pin)
    {
        if (!pin) {
            throw 0;
        } else if (!pin.mNode) {
            retVal = mArguments.at(pin.mIndex);
        } else {
            mGraph->node(pin.mNode)->interpretRead(*this, retVal, mData[pin.mNode - 1], pin.mIndex, pin.mGroup);
        }
    }

    void NodeInterpreterStateBase::write(Pin pin, const ValueType &v)
    {
        mGraph->node(pin.mNode)->interpretWrite(*this, mData[pin.mNode - 1], v, pin.mIndex, pin.mGroup);
    }

    void NodeInterpreterStateBase::read(ValueType &retVal, uint32_t dataProvider)
    {
        read(retVal, mGraph->mDataInPins[dataProvider].mSource);
    }

    void NodeInterpreterStateBase::write(uint32_t dataReceiver, const ValueType &v)
    {
        write(mGraph->mDataOutPins[dataReceiver].mTarget, v);
    }

    const NodeGraph *NodeInterpreterStateBase::graph() const
    {
        return mGraph;
    }

    std::unique_ptr<NodeInterpreterData> &NodeInterpreterStateBase::data(uint32_t index)
    {
        return mData[index - 1];
    }

    bool NodeInterpreterStateBase::readVar(ValueType &result, std::string_view name, bool recursive)
    {
        bool gotValue = false;
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                bool hadValue = gotValue;
                gotValue = data->readVar(result, name);
                assert(!hadValue || !gotValue);
            }
        }
        return gotValue;
    }

    bool NodeInterpreterStateBase::writeVar(std::string_view name, const ValueType &v)
    {
        bool gotValue = false;
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                bool hadValue = gotValue;
                gotValue = data->writeVar(name, v);
                assert(!hadValue || !gotValue);
            }
        }
        return gotValue;
    }

    std::vector<std::string_view> NodeInterpreterStateBase::variables()
    {
        std::vector<std::string_view> variables;
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                std::ranges::move(data->variables(), std::inserter(variables, variables.end()));
            }
        }
        return variables;
    }

    void NodeInterpreterStateBase::start(ArgumentList args)
    {
        mArguments = std::move(args);
        interpretImpl(*this, 0);
    }

}
}