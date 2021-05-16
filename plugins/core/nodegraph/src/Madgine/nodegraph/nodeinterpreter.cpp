#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "nodegraph.h"

namespace Engine {
namespace NodeGraph {

    NodeInterpreter::NodeInterpreter(const NodeGraph &graph)
        : mGraph(graph)
        , mData(graph.nodes().size())
    {
    }

    void NodeInterpreter::interpret(IndexType<uint32_t> &flowInOut)
    {
        const NodeBase *oldCurrentNode = mCurrentNode;

        TargetPin pin = mGraph.mFlowInPins[flowInOut].mTarget;

        while (pin && pin.mNode) {
            mCurrentNode = mGraph.node(pin.mNode);
            IndexType<uint32_t> flowIndex = pin.mIndex;
            mCurrentNode->interpret(*this, flowIndex, mData[pin.mNode - 1]);
            if (flowIndex)
                pin = mCurrentNode->flowOutTarget(flowIndex);
            else
                pin = {};
        }

        mCurrentNode = oldCurrentNode;

        flowInOut = pin.mIndex;
    }

    void NodeInterpreter::read(ValueType &retVal, uint32_t dataInIndex)
    {
        TargetPin pin = mCurrentNode->dataInSource(dataInIndex);
        mGraph.node(pin.mNode)->interpretRead(*this, retVal, pin.mIndex, mData[pin.mNode - 1]);
    }

    void NodeInterpreter::write(uint32_t dataOutIndex, const ValueType &v)
    {
        TargetPin pin = mCurrentNode->dataOutTarget(dataOutIndex);
        if (pin)
            mGraph.node(pin.mNode)->interpretWrite(*this, pin.mIndex, mData[pin.mNode - 1], v);
    }

}
}