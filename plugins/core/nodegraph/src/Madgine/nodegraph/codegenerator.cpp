#include "../nodegraphlib.h"

#include "codegenerator.h"

#include "nodegraph.h"

namespace Engine {
namespace NodeGraph {

    CodeGenerator::CodeGenerator(const NodeGraph &graph)
        : mGraph(graph)
        , mData(graph.nodes().size())
    {
    }

    void CodeGenerator::generate(IndexType<uint32_t> &flowInOut, const NodeBase *startNode)
    {
        Pin pin;
        if (startNode) {
            pin.mIndex = flowInOut;
            pin.mNode = mGraph.nodeIndex(startNode);
        } else {
            pin = mGraph.mFlowOutPins[flowInOut].mTarget;
        }

        generate(pin);

        flowInOut = pin.mIndex;
    }

    void CodeGenerator::generate(Pin &pin)
    {
        const NodeBase *oldCurrentNode = mCurrentNode;

        while (pin && pin.mNode) {
            mCurrentNode = mGraph.node(pin.mNode);
            IndexType<uint32_t> flowIndex = pin.mIndex;
            mCurrentNode->generate(*this, flowIndex, mData[pin.mNode - 1]);
            if (flowIndex)
                pin = mCurrentNode->flowOutTarget(flowIndex);
            else
                pin = {};
        }

        mCurrentNode = oldCurrentNode;
    }

    CodeGen::Statement CodeGenerator::read(uint32_t dataInIndex)
    {
        Pin pin = mCurrentNode->dataInSource(dataInIndex);
        if (!pin.mNode) { 
            //retVal = mCurrentArguments->at(pin.mIndex);
            throw 0;
        } else {
            const NodeBase *oldCurrentNode = mCurrentNode;
            mCurrentNode = mGraph.node(pin.mNode);
            CodeGen::Statement result = mCurrentNode->generateRead(*this, pin.mIndex, mData[pin.mNode - 1]);
            mCurrentNode = oldCurrentNode;
            return result;
        }
    }

    CodeGen::Statement CodeGenerator::write(uint32_t dataOutIndex, CodeGen::Statement statement)
    {
        Pin pin = mCurrentNode->dataOutTarget(dataOutIndex);
        if (pin)
            return mGraph.node(pin.mNode)->generateWrite(*this, pin.mIndex, mData[pin.mNode - 1]);
        else
            throw 0;
    }

}
}