#include "../nodegraphlib.h"

#include "Modules/keyvalueutil/valuetypeserialize.h"

#include "nodegraphprototype.h"

#include "nodeprototypebase.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Interfaces/filesystem/api.h"

#include "filesystem/filesystemlib.h"
#include "filesystem/filemanager.h"

#include "Modules/serialize/streams/serializestream.h"

#include "Modules/serialize/serializetable_impl.h"

#include "xml/xmllib.h"
#include "xml/xmlformatter.h"

#include "nodeprototypecollector.h"

#include "nodegraphinstance.h"

#include "Modules/uniquecomponent/uniquecomponentregistry.h"

METATABLE_BEGIN(Engine::NodeGraph::NodeGraphPrototype)
METATABLE_END(Engine::NodeGraph::NodeGraphPrototype)

SERIALIZETABLE_BEGIN(Engine::NodeGraph::NodeGraphPrototype)
FIELD(mNodes, Serialize::ParentCreator<&Engine::NodeGraph::NodeGraphPrototype::createNodeTuple, &Engine::NodeGraph::NodeGraphPrototype::storeNodeCreationData>)
FIELD(mFlowInPins)
FIELD(mDataOutPins)
SERIALIZETABLE_END(Engine::NodeGraph::NodeGraphPrototype)

namespace Engine {
namespace NodeGraph {
    NodeGraphPrototype::NodeGraphPrototype()
    {
    }
    NodeGraphPrototype::NodeGraphPrototype(NodeGraphPrototype &&other) = default;
    NodeGraphPrototype::~NodeGraphPrototype()
    {
    }
    void NodeGraphPrototype::loadFromFile(const Filesystem::Path &path)
    {
        mPath = path;
        if (Filesystem::exists(mPath)) {
            Filesystem::FileManager mgr("Graph-Serializer");
            Serialize::SerializeInStream in = mgr.openRead(mPath, std::make_unique<XML::XMLFormatter>());
            readState(in, "Graph");
            std::vector<bool> outFlows;
            std::vector<std::optional<DataOutPinPrototype>> inPins;
            for (NodePrototypeBase *node : uniquePtrToPtr(mNodes)) {
                for (size_t i = 0; i < node->dataInCount(); ++i) {
                    TargetPin pin = node->dataInSource(i);
                    if (pin.mNode == this) {
                        if (inPins.size() <= pin.mIndex)
                            inPins.resize(pin.mIndex + 1);
                        inPins[pin.mIndex] = { node->dataInExpectedType(i) };
                    }
                }
                for (size_t i = 0; i < node->flowOutCount(); ++i) {
                    TargetPin pin = node->flowOutTarget(i);
                    if (pin.mNode == this) {
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
            for (const std::optional<DataOutPinPrototype> &pin : inPins) {
                assert(pin);
            }
            mDataInPins.clear();
            std::transform(std::make_move_iterator(inPins.begin()), std::make_move_iterator(inPins.end()), std::back_inserter(mDataInPins),
                [](std::optional<DataOutPinPrototype> &&pin) { return DataOutPinPrototype {std::move(*pin)}; });
        } else {
            saveToFile();
        }
    }

    void NodeGraphPrototype::saveToFile()
    {
        Filesystem::FileManager mgr("Graph-Serializer");
        Serialize::SerializeOutStream out = mgr.openWrite(mPath, std::make_unique<XML::XMLFormatter>());
        writeState(out, "Graph");
    }

    NodePrototypeBase *NodeGraphPrototype::addNode(std::unique_ptr<NodePrototypeBase> node)
    {
        NodePrototypeBase *ptr = node.get();
        mNodes.push_back(std::move(node));
        return ptr;
    }

    NodePrototypeBase *NodeGraphPrototype::addNode(const std::string_view &name)
    {
        return addNode(createNode(name));
    }

    void NodeGraphPrototype::removeNode(NodePrototypeBase *node)
    {
        mNodes.erase(std::find_if(mNodes.begin(), mNodes.end(), [=](const std::unique_ptr<NodePrototypeBase> &p) { return p.get() == node; }));
    }

    const std::vector<std::unique_ptr<NodePrototypeBase>> &NodeGraphPrototype::nodes() const
    {
        return mNodes;
    }

    void NodeGraphPrototype::connectFlow(NodePrototypeBase *source, size_t sourceIndex, GraphExecutable *target, size_t targetIndex)
    {
        if (!target) {
            assert(mFlowOutPinCount >= targetIndex);
            if (mFlowOutPinCount == targetIndex)
                ++mFlowOutPinCount;
            target = this;
        }
        if (!source) {
            assert(mFlowInPins.size() >= sourceIndex);
            if (mFlowInPins.size() == sourceIndex)
                mFlowInPins.resize(sourceIndex + 1);
            mFlowInPins[sourceIndex] = { target, targetIndex };
        } else {
            source->connectFlow(sourceIndex, target, targetIndex);
        }
    }

    void NodeGraphPrototype::connectData(GraphExecutable *source, size_t sourceIndex, NodePrototypeBase *target, size_t targetIndex)
    {
        assert(source || target);
        if (!source) {
            assert(mDataInPins.size() >= sourceIndex);
            if (mDataInPins.size() == sourceIndex)
                mDataInPins.emplace_back(DataOutPinPrototype { target->dataInExpectedType(targetIndex) });
            source = this;
        }
        if (!target) {
            assert(mDataOutPins.size() >= targetIndex);
            if (mDataOutPins.size() == targetIndex)
                mDataOutPins.emplace_back(DataInPinPrototype { { source, sourceIndex } });
        } else {
            target->connectData(targetIndex, source, sourceIndex);
        }
    }

    void NodeGraphPrototype::execute(NodeInstance *instance, uint32_t flowIndex, ArgumentList *out) const
    {
    }

    NodeGraphInstance NodeGraphPrototype::createInstance()
    {
        return {};
    }

    std::unique_ptr<NodePrototypeBase> NodeGraphPrototype::createNode(const std::string_view &name)
    {
        return NodePrototypeRegistry::getConstructor(sNodesByName().at(name))();
    }

    std::tuple<std::unique_ptr<NodePrototypeBase>> NodeGraphPrototype::createNodeTuple(const std::string &name)
    {
        return { createNode(name) };
    }

    std::tuple<std::pair<const char *, std::string_view>> NodeGraphPrototype::storeNodeCreationData(const std::unique_ptr<NodePrototypeBase> &node) const
    {
        return std::make_tuple(std::make_pair("type", node->name()));
    }

}
}