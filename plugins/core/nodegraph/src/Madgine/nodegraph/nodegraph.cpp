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
FIELD(mFlowOutPins)
FIELD(mDataInPins)
FIELD(mDataOutPins)
SERIALIZETABLE_END(Engine::NodeGraph::NodeGraph)

namespace Engine {
namespace NodeGraph {
    NodeGraph::NodeGraph()
    {
    }

    NodeGraph::NodeGraph(const NodeGraph &other)
        : mFlowInPins(other.mFlowInPins)
        , mFlowOutPins(other.mFlowOutPins)
        , mDataReceiverPins(other.mDataReceiverPins)
        , mDataProviderPins(other.mDataProviderPins)
        , mDataInPins(other.mDataInPins)
        , mDataOutPins(other.mDataOutPins)
        , mPath(other.mPath)
    {
        mNodes.reserve(other.mNodes.size());
        std::transform(other.mNodes.begin(), other.mNodes.end(), std::back_inserter(mNodes), [&](const std::unique_ptr<NodeBase> &node) { return node->clone(*this); });
    }

    NodeGraph::NodeGraph(NodeGraph &&other) = default;
    NodeGraph::~NodeGraph()
    {
    }

    NodeGraph &NodeGraph::operator=(const NodeGraph &other)
    {
        mPath = other.mPath;
        mFlowInPins = other.mFlowInPins;
        mFlowOutPins = other.mFlowOutPins;
        mDataInPins = other.mDataInPins;
        mDataOutPins = other.mDataOutPins;
        mDataProviderPins = other.mDataProviderPins;
        mDataReceiverPins = other.mDataReceiverPins;

        mNodes.clear();
        mNodes.reserve(other.mNodes.size());
        std::transform(other.mNodes.begin(), other.mNodes.end(), std::back_inserter(mNodes), [&](const std::unique_ptr<NodeBase> &node) { return node->clone(*this); });

        ++mGeneration;

        return *this;
    }

    std::string_view NodeGraph::name() const
    {
        return mPath.stem();
    }

    uint32_t NodeGraph::generation() const
    {
        return mGeneration;
    }

    Serialize::StreamResult NodeGraph::loadFromFile(const Filesystem::Path &path)
    {
        mPath = path;
        if (Filesystem::exists(mPath)) {
            Filesystem::FileManager mgr("Graph-Serializer");
            Serialize::SerializeInStream in = mgr.openRead(mPath, std::make_unique<Serialize::XMLFormatter>());
            STREAM_PROPAGATE_ERROR(Serialize::read(in, *this, "Graph", {}, Serialize::StateTransmissionFlags_ApplyMap));

            for (NodeBase *node : uniquePtrToPtr(mNodes))
                node->setup();

            uint32_t i = 0;
            for (FlowOutPinPrototype &flowOut : mFlowOutPins) {
                if (flowOut.mTarget) {
                    this->node(flowOut.mTarget.mNode)->mFlowInPins[flowOut.mTarget.mIndex].mSources.push_back({ 0, i });
                }
                ++i;
            }

            i = 0;
            for (DataInPinPrototype &dataIn : mDataInPins) {
                if (dataIn.mSource) {
                    this->node(dataIn.mSource.mNode)->mDataProviderPins[dataIn.mSource.mIndex].mTargets.push_back({ 0, i });
                }
                ++i;
            }

            i = 0;
            for (DataOutPinPrototype &dataOut : mDataOutPins) {
                if (dataOut.mTarget) {
                    this->node(dataOut.mTarget.mNode)->mFlowInPins[dataOut.mTarget.mIndex].mSources.push_back({ 0, i });
                }
                ++i;
            }

            std::vector<std::optional<FlowInPinPrototype>> inFlows;
            std::vector<std::optional<DataProviderPinPrototype>> providerPins;
            std::vector<std::optional<DataReceiverPinPrototype>> receiverPins;
            for (NodeBase *node : uniquePtrToPtr(mNodes)) {
                for (uint32_t i = 0; i < node->dataInCount(); ++i) {
                    Pin pin = node->dataInSource(i);
                    if (pin) {
                        if (!pin.mNode) {
                            if (providerPins.size() <= pin.mIndex)
                                providerPins.resize(pin.mIndex + 1);
                            providerPins[pin.mIndex] = DataProviderPinPrototype { { { nodeIndex(node), i } } };
                        } else {
                            this->node(pin.mNode)->mDataProviderPins[pin.mIndex].mTargets.push_back({ nodeIndex(node), i });
                        }
                    }
                }
                for (uint32_t i = 0; i < node->dataOutCount(); ++i) {
                    Pin pin = node->dataOutTarget(i);
                    if (pin) {
                        if (!pin.mNode) {
                            if (receiverPins.size() <= pin.mIndex)
                                receiverPins.resize(pin.mIndex + 1);
                            receiverPins[pin.mIndex] = DataReceiverPinPrototype { { { nodeIndex(node), i } } };
                        } else {
                            NodeBase *targetNode = this->node(pin.mNode);
                            if (targetNode->mDataReceiverPins.size() <= pin.mIndex) {
                                node->mDataOutPins[i].mTarget = {};
                            } else {
                                targetNode->mDataReceiverPins[pin.mIndex].mSources.push_back({ nodeIndex(node), i });
                            }
                        }
                    }
                }
                for (size_t i = 0; i < node->flowOutCount(); ++i) {
                    Pin pin = node->flowOutTarget(i);
                    if (pin) {
                        if (!pin.mNode) {
                            if (inFlows.size() <= pin.mIndex)
                                inFlows.resize(pin.mIndex + 1);
                            inFlows[pin.mIndex] = FlowInPinPrototype { { { nodeIndex(node), i } } };
                        } else {
                            NodeBase *targetNode = this->node(pin.mNode);
                            if (targetNode->mFlowInPins.size() <= pin.mIndex) {
                                node->mFlowOutPins[i].mTarget = {};
                            } else {
                                targetNode->mFlowInPins[pin.mIndex].mSources.push_back({ nodeIndex(node), i });
                            }
                        }
                    }
                }
            }
            for (const std::optional<FlowInPinPrototype> &pin : inFlows) {
                assert(pin);
            }
            for (const std::optional<DataProviderPinPrototype> &pin : providerPins) {
                assert(pin);
            }
            for (const std::optional<DataReceiverPinPrototype> &pin : receiverPins) {
                assert(pin);
            }
            mFlowInPins.clear();
            std::transform(std::make_move_iterator(inFlows.begin()), std::make_move_iterator(inFlows.end()), std::back_inserter(mFlowInPins),
                [](std::optional<FlowInPinPrototype> &&pin) { return FlowInPinPrototype { std::move(*pin) }; });
            mDataProviderPins.clear();
            std::transform(std::make_move_iterator(providerPins.begin()), std::make_move_iterator(providerPins.end()), std::back_inserter(mDataProviderPins),
                [](std::optional<DataProviderPinPrototype> &&pin) { return DataProviderPinPrototype { std::move(*pin) }; });
            mDataReceiverPins.clear();
            std::transform(std::make_move_iterator(receiverPins.begin()), std::make_move_iterator(receiverPins.end()), std::back_inserter(mDataReceiverPins),
                [](std::optional<DataReceiverPinPrototype> &&pin) { return DataReceiverPinPrototype { std::move(*pin) }; });

        } else {
            saveToFile();
        }
        return {};
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

    NodeBase *NodeGraph::addNode(std::string_view name)
    {
        return addNode(createNode(name));
    }

    void NodeGraph::removeNode(uint32_t index)
    {
        assert(index != 0);
        uint32_t oldIndex = mNodes.size();
        std::swap(mNodes[index - 1], mNodes.back());
        mNodes.pop_back();

        for (const std::unique_ptr<NodeBase> &node : mNodes)
            node->onNodeRemove(oldIndex, index);

        for (FlowOutPinPrototype &pin : mFlowOutPins) {
            if (pin.mTarget.mNode == index)
                pin.mTarget = {};
            else if (pin.mTarget.mNode == oldIndex)
                pin.mTarget.mNode = index;
        }
        for (DataInPinPrototype &pin : mDataInPins) {
            if (pin.mSource.mNode == index)
                pin.mSource = {};
            else if (pin.mSource.mNode == oldIndex)
                pin.mSource.mNode = index;
        }
        for (DataOutPinPrototype &pin : mDataOutPins) {
            if (pin.mTarget.mNode == index)
                pin.mTarget = {};
            else if (pin.mTarget.mNode == oldIndex)
                pin.mTarget.mNode = index;
        }

        for (FlowInPinPrototype &pin : mFlowInPins) {
            for (Pin &source : pin.mSources) {
                if (source.mNode == index)
                    source = {};
                else if (source.mNode == oldIndex)
                    source.mNode = index;
            }
        }
        for (DataReceiverPinPrototype &pin : mDataReceiverPins) {
            for (Pin &source : pin.mSources) {
                if (source.mNode == index)
                    source = {};
                else if (source.mNode == oldIndex)
                    source.mNode = index;
            }
        }

        for (uint32_t i = 0; i < mDataProviderPins.size();) {
            DataProviderPinPrototype &pin = mDataProviderPins[i];
            for (auto it = pin.mTargets.begin(); it != pin.mTargets.end();) {
                if (it->mNode == index) {
                    it = pin.mTargets.erase(it);
                } else if (it->mNode == oldIndex) {
                    it->mNode = index;
                    ++it;
                } else {
                    ++it;
                }
            }
            if (checkDataProvider(i))
                ++i;
        }
    }

    const std::vector<std::unique_ptr<NodeBase>> &NodeGraph::nodes() const
    {
        return mNodes;
    }

    const NodeBase *NodeGraph::node(IndexType<uint32_t, 0> index) const
    {
        if (index)
            return mNodes[index - 1].get();
        else
            return nullptr;
    }

    NodeBase *NodeGraph::node(IndexType<uint32_t, 0> index)
    {
        if (index)
            return mNodes[index - 1].get();
        else
            return nullptr;
    }

    uint32_t NodeGraph::nodeIndex(const NodeBase *node) const
    {
        if (!node)
            return 0;
        return std::find_if(mNodes.begin(), mNodes.end(), [=](const std::unique_ptr<NodeBase> &ptr) { return ptr.get() == node; }) - mNodes.begin() + 1;
    }

    Pin NodeGraph::flowOutTarget(Pin source)
    {
        if (source.mNode)
            return node(source.mNode)->flowOutTarget(source.mIndex);
        if (source.mIndex == mFlowInPins.size())
            return {};
        return mFlowOutPins[source.mIndex].mTarget;
    }

    uint32_t NodeGraph::flowOutMask(Pin source, bool bidir)
    {
        if (source.mNode)
            return node(source.mNode)->flowOutMask(source.mIndex, bidir);
        if (source.mIndex == mFlowOutPins.size())
            return NodeExecutionMask::ALL;
        return node(mFlowOutPins[source.mIndex].mTarget.mNode)->flowInMask(mFlowOutPins[source.mIndex].mTarget.mIndex, false);
    }

    uint32_t NodeGraph::NodeGraph::flowInMask(Pin target, bool bidir)
    {
        if (target.mNode)
            return node(target.mNode)->flowInMask(target.mIndex, bidir);
        if (target.mIndex == mFlowInPins.size())
            return NodeExecutionMask::ALL;
        return node(mFlowInPins[target.mIndex].mSources.front().mNode)->flowOutMask(mFlowInPins[target.mIndex].mSources.front().mIndex, false);
    }

    Pin NodeGraph::dataInSource(Pin target)
    {
        if (target.mNode)
            return node(target.mNode)->dataInSource(target.mIndex);
        if (target.mIndex == mDataInPins.size())
            return {};
        return mDataInPins[target.mIndex].mSource;
    }

    Pin NodeGraph::dataOutTarget(Pin source)
    {
        if (source.mNode)
            return node(source.mNode)->dataOutTarget(source.mIndex);
        if (source.mIndex == mDataOutPins.size())
            return {};
        return mDataOutPins[source.mIndex].mTarget;
    }

    ExtendedValueTypeDesc NodeGraph::dataReceiverType(Pin source, bool bidir)
    {
        if (source.mNode)
            return node(source.mNode)->dataReceiverType(source.mIndex, bidir);
        if (!bidir || source.mIndex == mDataReceiverPins.size())
            return { ExtendedValueTypeEnum::GenericType };
        Pin pin = mDataReceiverPins[source.mIndex].mSources.front();
        return node(pin.mNode)->dataOutType(pin.mIndex, false);
    }

    ExtendedValueTypeDesc NodeGraph::dataProviderType(Pin target, bool bidir)
    {
        if (target.mNode)
            return node(target.mNode)->dataProviderType(target.mIndex, bidir);
        if (!bidir || target.mIndex == mDataProviderPins.size())
            return { ExtendedValueTypeEnum::GenericType };
        Pin pin = mDataProviderPins[target.mIndex].mTargets.front();
        return node(pin.mNode)->dataInType(pin.mIndex, false);
    }

    ExtendedValueTypeDesc NodeGraph::dataInType(Pin source, bool bidir)
    {
        if (source.mNode)
            return node(source.mNode)->dataInType(source.mIndex, bidir);
        if (!bidir || source.mIndex == mDataInPins.size())
            return { ExtendedValueTypeEnum::GenericType };
        return node(mDataInPins[source.mIndex].mSource.mNode)->dataProviderType(mDataInPins[source.mIndex].mSource.mIndex);
    }

    ExtendedValueTypeDesc NodeGraph::dataOutType(Pin target, bool bidir)
    {
        if (target.mNode)
            return node(target.mNode)->dataInType(target.mIndex, bidir);
        if (!bidir || target.mIndex == mDataOutPins.size())
            return { ExtendedValueTypeEnum::GenericType };
        return node(mDataOutPins[target.mIndex].mTarget.mNode)->dataReceiverType(mDataOutPins[target.mIndex].mTarget.mIndex);
    }

    uint32_t NodeGraph::dataReceiverMask(Pin source, bool bidir)
    {
        if (source.mNode)
            return node(source.mNode)->dataReceiverMask(source.mIndex, bidir);
        if (!bidir || source.mIndex == mDataReceiverPins.size())
            return NodeExecutionMask::ALL;
        Pin pin = mDataReceiverPins[source.mIndex].mSources.front();
        return node(pin.mNode)->dataOutMask(pin.mIndex, false);
    }

    uint32_t NodeGraph::dataProviderMask(Pin target, bool bidir)
    {
        if (target.mNode)
            return node(target.mNode)->dataProviderMask(target.mIndex, bidir);
        if (!bidir || target.mIndex == mDataProviderPins.size())
            return NodeExecutionMask::ALL;
        Pin pin = mDataProviderPins[target.mIndex].mTargets.front();
        return node(pin.mNode)->dataInMask(pin.mIndex, false);
    }

    uint32_t NodeGraph::dataInMask(Pin source, bool bidir)
    {
        if (source.mNode)
            return node(source.mNode)->dataInMask(source.mIndex, bidir);
        if (!bidir || source.mIndex == mDataInPins.size())
            return NodeExecutionMask::ALL;
        return node(mDataInPins[source.mIndex].mSource.mNode)->dataProviderMask(mDataInPins[source.mIndex].mSource.mIndex, false);
    }

    uint32_t NodeGraph::dataOutMask(Pin target, bool bidir)
    {
        if (target)
            return node(target.mNode)->dataInMask(target.mIndex, bidir);
        if (!bidir || target.mIndex == mDataOutPins.size())
            return NodeExecutionMask::ALL;
        return node(mDataOutPins[target.mIndex].mTarget.mNode)->dataReceiverMask(mDataOutPins[target.mIndex].mTarget.mIndex, false);
    }

    std::string_view NodeGraph::flowOutName(Pin source)
    {
        return node(source.mNode)->flowOutName(source.mIndex);
    }

    std::string_view NodeGraph::flowInName(Pin target)
    {
        return node(target.mNode)->flowInName(target.mIndex);
    }

    std::string_view NodeGraph::dataReceiverName(Pin source)
    {
        return node(source.mNode)->dataReceiverName(source.mIndex);
    }

    std::string_view NodeGraph::dataProviderName(Pin target)
    {
        return target.mNode ? node(target.mNode)->dataProviderName(target.mIndex) : "input";
    }

    std::string_view NodeGraph::dataInName(Pin source)
    {
        return node(source.mNode)->dataInName(source.mIndex);
    }

    std::string_view NodeGraph::dataOutName(Pin target)
    {
        return node(target.mNode)->dataOutName(target.mIndex);
    }

    void NodeGraph::connectFlow(Pin source, Pin target)
    {
        if (!target.mNode) {
            assert(mFlowInPins.size() >= target.mIndex);
            if (mFlowInPins.size() == target.mIndex)
                mFlowInPins.emplace_back();
            mFlowInPins[target.mIndex].mSources.push_back(source);
        } else {
            node(target.mNode)->onFlowInUpdate(target.mIndex, source, CONNECT);
            node(target.mNode)->mFlowInPins[target.mIndex].mSources.push_back(source);
        }
        if (!source.mNode) {
            assert(mFlowOutPins.size() >= source.mIndex);
            if (mFlowOutPins.size() == source.mIndex)
                mFlowOutPins.resize(source.mIndex + 1);
            mFlowOutPins[source.mIndex] = { target };
        } else {
            node(source.mNode)->onFlowOutUpdate(source.mIndex, target, CONNECT);
            node(source.mNode)->mFlowOutPins[source.mIndex].mTarget = target;
        }
    }

    void NodeGraph::connectDataIn(Pin target, Pin source)
    {
        if (!source.mNode) {
            assert(mDataProviderPins.size() >= source.mIndex);
            if (mDataProviderPins.size() == source.mIndex)
                mDataProviderPins.emplace_back();
            mDataProviderPins[source.mIndex].mTargets.push_back(target);
        } else {
            node(source.mNode)->onDataProviderUpdate(source.mIndex, target, CONNECT);
            node(source.mNode)->mDataProviderPins[source.mIndex].mTargets.push_back(target);
        }

        if (!target.mNode) {
            assert(mDataInPins.size() <= target.mIndex);
            if (mDataInPins.size() == target.mIndex)
                mDataInPins.emplace_back(DataInPinPrototype { source });
        } else {
            node(target.mNode)->onDataInUpdate(target.mIndex, source, CONNECT);
            node(target.mNode)->mDataInPins[target.mIndex].mSource = source;
        }
    }

    void NodeGraph::connectDataOut(Pin source, Pin target)
    {
        if (!target.mNode) {
            assert(mDataReceiverPins.size() >= target.mIndex);
            if (mDataReceiverPins.size() == target.mIndex)
                mDataReceiverPins.emplace_back();
            mDataReceiverPins[source.mIndex].mSources.push_back(source);
        } else {
            node(target.mNode)->onDataReceiverUpdate(target.mIndex, source, CONNECT);
            node(target.mNode)->mDataReceiverPins[target.mIndex].mSources.push_back(source);
        }

        if (!source.mNode) {
            assert(mDataOutPins.size() <= source.mIndex);
            if (mDataOutPins.size() == source.mIndex)
                mDataOutPins.emplace_back(DataOutPinPrototype { target });
        } else {
            node(source.mNode)->onDataOutUpdate(source.mIndex, target, CONNECT);
            node(source.mNode)->mDataOutPins[source.mIndex].mTarget = target;
        }
    }

    void NodeGraph::disconnectFlow(Pin source)
    {
        Pin target;
        if (!source.mNode) {
            target = mFlowOutPins[source.mIndex].mTarget;
            mFlowOutPins.erase(mFlowOutPins.begin() + source.mIndex);
        } else {
            target = node(source.mNode)->mFlowOutPins[source.mIndex].mTarget;
            node(source.mNode)->mFlowOutPins[source.mIndex] = {};
            node(source.mNode)->onFlowOutUpdate(source.mIndex, target, DISCONNECT);
        }

        if (!target.mNode) {
            throw 0;
        } else {
            /* auto result = */ std::erase(node(target.mNode)->mFlowInPins[target.mIndex].mSources, source);
            /*assert(result == 1);*/
            node(target.mNode)->onFlowInUpdate(target.mIndex, source, DISCONNECT);
        }
    }

    void NodeGraph::disconnectDataIn(Pin target)
    {
        Pin source;
        if (!target.mNode) {
            source = mDataInPins[target.mIndex].mSource;
            mDataInPins.erase(mDataInPins.begin() + target.mIndex);
        } else {
            source = node(target.mNode)->mDataInPins[target.mIndex].mSource;
            node(target.mNode)->mDataInPins[target.mIndex] = {};
            node(target.mNode)->onDataInUpdate(target.mIndex, source, DISCONNECT);
        }

        if (!source.mNode) {
            std::erase(mDataProviderPins[source.mIndex].mTargets, target);
            checkDataProvider(source.mIndex);
        } else {
            /* auto result = */ std::erase(node(source.mNode)->mDataProviderPins[source.mIndex].mTargets, target);
            /* assert(result == 1);*/
            node(source.mNode)->onDataProviderUpdate(source.mIndex, target, DISCONNECT);
        }
    }

    void NodeGraph::disconnectDataOut(Pin source)
    {
        Pin target;
        if (!source) {
            target = mDataOutPins[source.mIndex].mTarget;
            mDataOutPins.erase(mDataOutPins.begin() + source.mIndex);
        } else {
            target = node(source.mNode)->mDataOutPins[source.mIndex].mTarget;
            node(source.mNode)->mDataOutPins[source.mIndex] = {};
            node(source.mNode)->onDataOutUpdate(source.mIndex, target, DISCONNECT);
        }

        if (!target.mNode) {
            throw 0;
        } else {
            /* auto result = */ std::erase(node(target.mNode)->mDataReceiverPins[target.mIndex].mSources, source);
            /* assert(result == 1);*/
            node(target.mNode)->onDataReceiverUpdate(target.mIndex, source, DISCONNECT);
        }
    }

    bool NodeGraph::checkDataProvider(uint32_t index)
    {
        if (mDataProviderPins[index].mTargets.empty()) {
            onDataProviderRemove(nullptr, index);
            mDataProviderPins.erase(mDataProviderPins.begin() + index);
            return false;
        }
        return true;
    }

    void NodeGraph::onDataProviderRemove(const NodeBase *node, uint32_t index)
    {
        Pin pin { nodeIndex(node), index };
        for (const std::unique_ptr<NodeBase> &ptr : mNodes) {
            if (ptr.get() != node)
                ptr->onDataProviderRemove(pin);
        }
    }

    const char *NodeGraph::nodeCreationNames(size_t index)
    {
        assert(index == 0);
        return "type";
    }

    std::unique_ptr<NodeBase> NodeGraph::createNode(std::string_view name)
    {
        return NodeRegistry::getConstructor(sNodesByName().at(name))(*this);
    }

    std::tuple<std::unique_ptr<NodeBase>> NodeGraph::createNodeTuple(std::string_view name)
    {
        return { createNode(name) };
    }

    std::tuple<std::string_view> NodeGraph::storeNodeCreationData(const std::unique_ptr<NodeBase> &node) const
    {
        return std::make_tuple(node->className());
    }
}
}
