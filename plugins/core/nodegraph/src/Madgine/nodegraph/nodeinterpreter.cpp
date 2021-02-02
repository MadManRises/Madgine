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

    uint32_t NodeInterpreter::interpret(uint32_t flowIn)
    {
        const NodeBase *oldCurrentNode = mCurrentNode;

        TargetPin pin = mGraph.mFlowInPins[flowIn].mTarget;

        while (pin && pin.mNode) {
            mCurrentNode = mGraph.node(pin.mNode);
            uint32_t flowOutIndex = mCurrentNode->interpret(*this, pin.mIndex, mData[pin.mNode - 1]);
            if (flowOutIndex != std::numeric_limits<uint32_t>::max())
                pin = mCurrentNode->flowOutTarget(flowOutIndex);
            else
                pin = {};
        }

        mCurrentNode = oldCurrentNode;

        return pin.mIndex;
    }

    ValueType NodeInterpreter::read(uint32_t dataInIndex)
    {
        TargetPin pin = mCurrentNode->dataInSource(dataInIndex);
        return mGraph.node(pin.mNode)->interpretRead(*this, pin.mIndex, mData[pin.mNode - 1]);
    }

    void NodeInterpreter::write(uint32_t dataOutIndex, const ValueType &v)
    {
        TargetPin pin = mCurrentNode->dataOutTarget(dataOutIndex);
        mGraph.node(pin.mNode)->interpretWrite(*this, pin.mIndex, mData[pin.mNode - 1], v);
    }

}
}