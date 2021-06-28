#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "nodegraph.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {
namespace NodeGraph {

    NodeInterpreter::NodeInterpreter(const NodeGraph *graph)
        : mGraph(graph)        
    {
    }

    void NodeInterpreter::interpret(IndexType<uint32_t> &flowInOut, const ArgumentList &args, const NodeBase *startNode)
    {
        assert(mGraph);

        Pin pin;
        if (startNode) {
            pin.mIndex = flowInOut;
            pin.mNode = mGraph->nodeIndex(startNode);
        } else {
            pin = mGraph->mFlowOutPins[flowInOut].mTarget;
        }

        interpret(pin, args);

        flowInOut = pin.mIndex;
    }

    void NodeInterpreter::interpret(Pin &pin, const ArgumentList &args)
    {
        assert(mGraph);

        if (mGraphGeneration != mGraph->generation()) {
            mData.clear();
            mData.resize(mGraph->nodes().size());
            mGraphGeneration = mGraph->generation();
        }
        assert(!mCurrentArguments);
        const ArgumentList *oldCurrentArguments = mCurrentArguments;

        mCurrentArguments = &args;

        branch(pin);

        mCurrentArguments = oldCurrentArguments;
    }

    void NodeInterpreter::branch(IndexType<uint32_t> flow)
    {
        Pin pin = mCurrentNode->flowOutTarget(flow);
        branch(pin);
    }

    void NodeInterpreter::branch(Pin &pin)
    {
        const NodeBase *oldCurrentNode = mCurrentNode;

        while (pin && pin.mNode) {
            mCurrentNode = mGraph->node(pin.mNode);
            IndexType<uint32_t> flowIndex = pin.mIndex;
            mCurrentNode->interpret(*this, flowIndex, mData[pin.mNode - 1]);
            if (flowIndex)
                pin = mCurrentNode->flowOutTarget(flowIndex);
            else
                pin = {};
        }

        mCurrentNode = oldCurrentNode;
    }

    void NodeInterpreter::read(ValueType &retVal, uint32_t dataInIndex)
    {
        Pin pin = mCurrentNode->dataInSource(dataInIndex);
        if (!pin.mNode) {
            retVal = mCurrentArguments->at(pin.mIndex);
        } else {
            const NodeBase *oldCurrentNode = mCurrentNode;
            mCurrentNode = mGraph->node(pin.mNode);
            mCurrentNode->interpretRead(*this, retVal, pin.mIndex, mData[pin.mNode - 1]);
            mCurrentNode = oldCurrentNode;
        }
    }

    void NodeInterpreter::write(uint32_t dataOutIndex, const ValueType &v)
    {
        Pin pin = mCurrentNode->dataOutTarget(dataOutIndex);
        if (pin)
            mGraph->node(pin.mNode)->interpretWrite(*this, pin.mIndex, mData[pin.mNode - 1], v);
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
        return mData[index-1];
    }

}
}