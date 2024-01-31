#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "nodegraph.h"

#include "Meta/keyvalue/valuetype.h"

#include "nodeexecution.h"

#include "Madgine/debug/debugger.h"

namespace Engine {
namespace NodeGraph {

    std::string NodeDebugLocation::toString() const
    {
        std::stringstream ss;
        if (mNode) {
            ss << mNode->className() << " [" << mNode << "]";
        } else {
            ss << "Graph [" << mInterpreter->graph() << "]";
        }
        return ss.str();
    }

    std::map<std::string_view, ValueType> NodeDebugLocation::localVariables() const
    {
        std::map<std::string_view, ValueType> values;
        for (std::string_view name : mInterpreter->variables()) {
            mInterpreter->readVar(values.try_emplace(name).first->second, name);
        }
        return values;
    }

    NodeInterpreterStateBase::NodeInterpreterStateBase(const NodeGraph *graph, NodeGraphLoader::Handle handle)
        : mGraph(graph)
        , mHandle(std::move(handle))
        , mDebugLocation(this)
    {

    }

    void NodeInterpreterStateBase::interpretImpl(Execution::VirtualReceiverBase<BehaviorError> &receiver, uint32_t flowIn)
    {
        interpretImpl(receiver, mGraph->mFlowOutPins[flowIn].mTarget);
    }

    void NodeInterpreterStateBase::interpretImpl(Execution::VirtualReceiverBase<BehaviorError> &receiver, Pin pin)
    {
        mDebugLocation.stepInto(parentDebugLocation());
        branch(receiver, pin);
    }

    void NodeInterpreterStateBase::branch(Execution::VirtualReceiverBase<BehaviorError> &receiver, Pin pin)
    {

        const NodeBase *node = nullptr;
        if (pin && pin.mNode) {
            node = mGraph->node(pin.mNode);
        }

        mDebugLocation.mNode = node;

        mDebugLocation.pass([=, &receiver](Debug::ContinuationMode mode) {
            if (pin && pin.mNode) {
                node->interpret({ *this, *node, receiver }, mData[pin.mNode - 1], pin.mIndex, pin.mGroup);
            } else {
                mDebugLocation.stepOut(parentDebugLocation());
                receiver.set_value();
            }
        },
            parentStopToken());
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
                if (data->readVar(result, name)) {
                    assert(!gotValue);
                    gotValue = true;
                }
            }
        }
        return gotValue;
    }

    bool NodeInterpreterStateBase::writeVar(std::string_view name, const ValueType &v)
    {
        bool gotValue = false;
        for (const std::unique_ptr<NodeInterpreterData> &data : mData) {
            if (data) {
                if (data->writeVar(name, v)) {
                    assert(!gotValue);
                    gotValue = true;
                }
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

    void NodeInterpreterStateBase::start()
    {
        auto callback = [this](bool) {
            mData.resize(mGraph->nodes().size());

            for (size_t i = 0; i < mData.size(); ++i) {
                mGraph->node(i + 1)->setupInterpret(*this, mData[i]);
            }

            interpretImpl(*this, 0);
        };

        if (mGraph) {
            callback(true);
        } else {
            assert(mHandle);
            mHandle.info()->loadingTask().then([this, callback { std::move(callback) }](bool result) {
                mGraph = mHandle;
                callback(result);
            },
                NodeGraphLoader::getSingleton().loadingTaskQueue());
        }
    }

    Debug::DebugLocation *NodeInterpreterStateBase::debugLocation()
    {
        return &mDebugLocation;
    }

    bool NodeDebugLocation::wantsPause() const
    {
        return true;
    }

}
}