#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "nodegraph.h"

#include "Meta/keyvalue/valuetype.h"

#include "nodeexecution.h"

namespace Engine {
namespace NodeGraph {

    NodeInterpreter::NodeInterpreter(const NodeGraph *graph, const ArgumentList &args)
        : mGraph(graph)
        , mArguments(args)
    {
    }

    void NodeInterpreter::start()
    {
        interpret(mGraph->mFlowOutPins[0].mTarget);
    }

    void NodeInterpreter::interpret(Pin pin)
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

        branch(*this, pin);
    }

    void NodeInterpreter::branch(Execution::VirtualReceiverBase<GenericResult> &receiver, uint32_t flow)
    {
        Pin pin = mCurrentNode->flowOutTarget(flow);
        branch(receiver, pin);
    }

    void NodeInterpreter::branch(Execution::VirtualReceiverBase<GenericResult> &receiver, Pin pin)
    {
        if (pin && pin.mNode) {
            const NodeBase *node = mGraph->node(pin.mNode);
            mCurrentNode = node;
            uint32_t flowIndex = pin.mIndex;
            node->interpret({ node, *this, receiver }, flowIndex, mData[pin.mNode - 1]);
        } else {
            receiver.set_value();
        }
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

    bool NodeInterpreter::resolveVar(OutRef<ValueType> &result, std::string_view name)
    {
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                bool hadValue = result.mPtr;
                bool gotValue = data->resolveVar(result, name);
                assert(!hadValue || !gotValue);
            }
        }
        return result.mPtr;
    }

}
}