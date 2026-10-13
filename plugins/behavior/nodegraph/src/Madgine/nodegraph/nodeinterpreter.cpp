#include "../nodegraphlib.h"

#include "nodeinterpreter.h"

#include "Meta/type/storageops.h"

#include "Madgine/debug/debugger.h"

#include "nodeexecution.h"
#include "nodegraph.h"

namespace Engine {
namespace Behavior {
    namespace NodeGraph {

        NodeInterpreterStateBase::NodeInterpreterStateBase(const NodeGraph *graph, NodeGraphLoader::Handle handle, ParameterTuple args)
            : NodeStateBase(this)
            , mGraph(graph)
            , mHandle(std::move(handle))
            , mArguments(std::move(args))
        {
        }

        void NodeInterpreterStateBase::branch(NodeStateBase &receiver, uint32_t flowIn)
        {
            branch(receiver, mGraph->mFlowOutPins[flowIn].mTarget);
        }

        void NodeInterpreterStateBase::branch(NodeStateBase &receiver, Pin pin)
        {
            if (pin && pin.mNode) {
                const NodeBase *node = mGraph->node(pin.mNode);
                receiver.mDebugLocation.mNode = node;
                receiver.mDebugLocation.mContinuation.pass(&receiver.mDebugLocation, receiver, [=, this](NodeStateBase &receiver) { node->interpret({ { { { *this }, *node } }, receiver }, mData[pin.mNode - 1], pin.mIndex, pin.mGroup); });
            } else {
                receiver.mDebugLocation.mNode = nullptr;
                receiver.mDebugLocation.mContinuation.returnValue(&receiver.mDebugLocation, receiver);
            }
        }

        Reflect::Result NodeInterpreterStateBase::read(Reflect::Value &retVal, Pin pin)
        {
            if (!pin) {
                throw 0;
            } else if (!pin.mNode) {
                assert(pin.mGroup < 2);
                if (pin.mGroup == 0) {
                    mArguments.get(retVal, pin.mIndex);
                    return {};
                } else {
                    return Reflect::get_reflect_contextual(*this, retVal, *mGraph->mContextualInputs[pin.mIndex].mDescriptor.mType->mType.mSecondary.mMetaTable);
                    throw 0; // TODO: Move into single group, resolve contextual on call
                    // std::string_view name = mGraph->mNamedInputs[pin.mIndex].mDescriptor.mName;
                    // return get_named_d(*this, name, retVal);
                }
            } else {
                return mGraph->node(pin.mNode)->interpretRead(*this, retVal, mData[pin.mNode - 1], pin.mIndex, pin.mGroup);
            }
        }

        Reflect::Result NodeInterpreterStateBase::read(Reflect::Value &retVal, uint32_t dataProvider)
        {
            return read(retVal, mGraph->mDataInPins[dataProvider].mSource);
        }

        const NodeGraph *NodeInterpreterStateBase::graph() const
        {
            return mGraph;
        }

        std::unique_ptr<NodeInterpreterData> &NodeInterpreterStateBase::data(uint32_t index)
        {
            return mData[index - 1];
        }

        /* bool NodeInterpreterStateBase::readVar(ValueType &result, std::string_view name, bool recursive)
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
        }*/

        void NodeInterpreterStateBase::start()
        {
            Debug::Continuation *parent = Debug::get_continuation(*this);
            mDebugLocation.mContinuation.setMode(parent && (parent->mode() == Debug::ContinuationMode::StepInto) ? Debug::ContinuationMode::Step : Debug::ContinuationMode::Continue);

            if (!mGraph)
                mGraph = mHandle;
            mData.resize(mGraph->nodes().size());

            for (size_t i = 0; i < mData.size(); ++i) {
                mGraph->node(i + 1)->setupInterpret(*this, mData[i]);
            }

            branch(*this, 0);
        }

        void NodeStateBase::stop()
        {
            if (mDebugLocation.mContinuation.stop())
                mDebugLocation.mContinuation.done(); // Nothing to do. It is the responsibility of child behaviors to listen to the stop source.
        }

    }
}
}