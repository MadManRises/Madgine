#include "../nodegraphlib.h"
#include "serialize/filesystem/filesystemlib.h"

#include "Meta/keyvalueutil/valuetypeserialize.h"

#include "nodegraph.h"

#include "nodebase.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Interfaces/filesystem/api.h"

#include "serialize/filesystem/filemanager.h"

#include "Meta/serialize/streams/serializestream.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Meta/serialize/formatter/xmlformatter.h"

#include "nodecollector.h"

#include "Modules/uniquecomponent/uniquecomponentregistry.h"

METATABLE_BEGIN(Engine::NodeGraph::NodeGraph)
METATABLE_END(Engine::NodeGraph::NodeGraph)

SERIALIZETABLE_BEGIN(Engine::NodeGraph::NodeGraph)
FIELD(mNodes, Serialize::ParentCreator<&Engine::NodeGraph::NodeGraph::nodeCreationNames, &Engine::NodeGraph::NodeGraph::createNodeTuple, &Engine::NodeGraph::NodeGraph::storeNodeCreationData>)
FIELD(mFlowInPins)
FIELD(mDataReceiverPins)
FIELD(mDataProviderPins)
SERIALIZETABLE_END(Engine::NodeGraph::NodeGraph)

namespace Engine {
namespace NodeGraph {
    NodeGraph::NodeGraph()
    {
    }
    NodeGraph::NodeGraph(NodeGraph &&other) = default;
    NodeGraph::~NodeGraph()
    {
    }
    void NodeGraph::loadFromFile(const Filesystem::Path &path)
    {
        mPath = path;
        if (Filesystem::exists(mPath)) {
            Filesystem::FileManager mgr("Graph-Serializer");
            Serialize::SerializeInStream in = mgr.openRead(mPath, std::make_unique<Serialize::XMLFormatter>());
            Serialize::read(in, *this, "Graph", {}, Serialize::StateTransmissionFlags_ApplyMap);
            std::vector<bool> outFlows;
            std::vector<std::optional<DataProviderPinPrototype>> inPins;
            std::vector<std::optional<DataReceiverPinPrototype>> outPins;
            for (NodeBase *node : uniquePtrToPtr(mNodes)) {
                for (uint32_t i = 0; i < node->dataInCount(); ++i) {
                    TargetPin pin = node->dataInSource(i);
                    if (pin && !pin.mNode) {
                        if (inPins.size() <= pin.mIndex)
                            inPins.resize(pin.mIndex + 1);
                        inPins[pin.mIndex] = { node->dataInType(i) };
                    }
                }
                for (uint32_t i = 0; i < node->dataOutCount(); ++i) {
                    TargetPin pin = node->dataOutTarget(i);
                    if (pin && !pin.mNode) {
                        if (outPins.size() <= pin.mIndex)
                            outPins.resize(pin.mIndex + 1);
                        outPins[pin.mIndex] = { node->dataOutType(i) };
                    }
                }
                for (size_t i = 0; i < node->flowOutCount(); ++i) {
                    TargetPin pin = node->flowOutTarget(i);
                    if (pin && !pin.mNode) {
                        if (mFlowOutPinCount <= pin.mIndex) {
                            outFlows.resize(pin.mIndex + 1);
                            mFlowOutPinCount = pin.mIndex + 1;
                        }
                        outFlows[pin.mIndex] = true;
                    }
                }
            }
            for (bool b : outFlows) {
                assert(b);
            }
            for (const std::optional<DataProviderPinPrototype> &pin : inPins) {
                assert(pin);
            }
            for (const std::optional<DataReceiverPinPrototype> &pin : outPins) {
                assert(pin);
            }
            mDataInPins.clear();
            std::transform(std::make_move_iterator(inPins.begin()), std::make_move_iterator(inPins.end()), std::back_inserter(mDataInPins),
                [](std::optional<DataProviderPinPrototype> &&pin) { return DataProviderPinPrototype { std::move(*pin) }; });
            mDataOutPins.clear();
            std::transform(std::make_move_iterator(outPins.begin()), std::make_move_iterator(outPins.end()), std::back_inserter(mDataOutPins),
                [](std::optional<DataReceiverPinPrototype> &&pin) { return DataReceiverPinPrototype { std::move(*pin) }; });
        } else {
            saveToFile();
        }
    }

    void NodeGraph::saveToFile()
    {
        Filesystem::FileManager mgr("Graph-Serializer");
        Serialize::SerializeOutStream out = mgr.openWrite(mPath, std::make_unique<Serialize::XMLFormatter>());
        Serialize::SerializableDataPtr { this }.writeState(out, "Graph");
    }

    NodeBase *NodeGraph::addNode(std::unique_ptr<NodeBase> node)
    {
        mNodes.push_back(std::move(node));
        return mNodes.back().get();
    }

    NodeBase *NodeGraph::addNode(const std::string_view &name)
    {
        return addNode(createNode(name));
    }

    void NodeGraph::removeNode(NodeBase *node)
    {
        mNodes.erase(std::find_if(mNodes.begin(), mNodes.end(), [=](const std::unique_ptr<NodeBase> &p) { return p.get() == node; }));
    }

    const std::vector<std::unique_ptr<NodeBase>> &NodeGraph::nodes() const
    {
        return mNodes;
    }
    
    const NodeBase *NodeGraph::node(uint32_t index) const
    {
        if (index)
            return mNodes[index - 1].get();
        else
            return nullptr;
    }

    NodeBase *NodeGraph::node(uint32_t index)
    {
        if (index)
            return mNodes[index - 1].get();
        else
            return nullptr;
    }

    TargetPin NodeGraph::flowOutTarget(uint32_t source, uint32_t sourceIndex)
    {
        if (source)
            return node(source)->flowOutTarget(sourceIndex);
        if (sourceIndex == mFlowInPins.size())
            return {};
        return mFlowInPins[sourceIndex].mTarget;
    }

    TargetPin NodeGraph::dataInSource(uint32_t target, uint32_t targetIndex)
    {
        if (target)
            return node(target)->dataInSource(targetIndex);
        if (targetIndex == mDataProviderPins.size())
            return {};
        return mDataProviderPins[targetIndex].mSource;
    }

    TargetPin NodeGraph::dataOutTarget(uint32_t source, uint32_t sourceIndex)
    {
        if (source)
            return node(source)->dataOutTarget(sourceIndex);
        if (sourceIndex == mDataReceiverPins.size())
            return {};
        return mDataReceiverPins[sourceIndex].mTarget;
    }

    void NodeGraph::connectFlow(uint32_t source, uint32_t sourceIndex, uint32_t target, uint32_t targetIndex)
    {
        if (!target) {
            assert(mFlowOutPinCount >= targetIndex);
            if (mFlowOutPinCount == targetIndex)
                ++mFlowOutPinCount;
        }
        if (!source) {
            assert(mFlowInPins.size() >= sourceIndex);
            if (mFlowInPins.size() == sourceIndex)
                mFlowInPins.resize(sourceIndex + 1);
            mFlowInPins[sourceIndex] = { target, targetIndex };
        } else {
            node(source)->connectFlow(sourceIndex, target, targetIndex);
        }
    }

    void NodeGraph::connectDataIn(uint32_t target, uint32_t targetIndex, uint32_t source, uint32_t sourceIndex)
    {
        assert(!target != !source);
        if (!target) {
            assert(mDataProviderPins.size() >= targetIndex);
            if (mDataProviderPins.size() == targetIndex)
                mDataProviderPins.emplace_back(DataInPinPrototype { source, sourceIndex });
        } else if (!source) {
            assert(mDataInPins.size() >= sourceIndex);
            if (mDataInPins.size() == sourceIndex)
                mDataInPins.emplace_back(DataProviderPinPrototype { node(target)->dataInType(targetIndex) });
            node(target)->connectDataIn(targetIndex, source, sourceIndex);
        } else {
            throw 0;
        }
    }

    void NodeGraph::connectDataOut(uint32_t source, uint32_t sourceIndex, uint32_t target, uint32_t targetIndex)
    {
        assert(!target != !source);
        if (!source) {
            assert(mDataReceiverPins.size() >= sourceIndex);
            if (mDataReceiverPins.size() == sourceIndex)
                mDataReceiverPins.emplace_back(DataOutPinPrototype { target, targetIndex });
        } else if (!target) {
            assert(mDataOutPins.size() >= targetIndex);
            if (mDataOutPins.size() == targetIndex)
                mDataOutPins.emplace_back(DataReceiverPinPrototype { node(source)->dataOutType(sourceIndex) });
            node(source)->connectDataOut(sourceIndex, target, targetIndex);
        } else {
            throw 0;
        }
    }

    void NodeGraph::disconnectFlow(uint32_t index)
    {
        mFlowInPins.erase(mFlowInPins.begin() + index);
    }

    void NodeGraph::disconnectDataIn(uint32_t index)
    {
        mDataProviderPins.erase(mDataProviderPins.begin() + index);
    }

    void NodeGraph::disconnectDataOut(uint32_t index)
    {
        mDataReceiverPins.erase(mDataReceiverPins.begin() + index);
    }

    const char *NodeGraph::nodeCreationNames(size_t index)
    {
        assert(index == 0);
        return "type";
    }

    std::unique_ptr<NodeBase> NodeGraph::createNode(const std::string_view &name)
    {
        return NodeRegistry::getConstructor(sNodesByName().at(name))(*this);
    }

    std::tuple<std::unique_ptr<NodeBase>> NodeGraph::createNodeTuple(const std::string &name)
    {
        return { createNode(name) };
    }

    std::tuple<std::string_view> NodeGraph::storeNodeCreationData(const std::unique_ptr<NodeBase> &node) const
    {
        return std::make_tuple(node->name());
    }

}
}
