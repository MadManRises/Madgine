#pragma once

#include "Interfaces/filesystem/path.h"

#include "nodebase.h"

#include "Meta/serialize/container/serializablecontainer.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeGraph : Serialize::SerializableDataUnit {

        SERIALIZABLEUNIT(NodeGraph);

        NodeGraph();
        NodeGraph(const NodeGraph &) = delete;
        NodeGraph(NodeGraph &&);
        ~NodeGraph();

        NodeGraph &operator=(const NodeGraph &) = delete;

        void loadFromFile(const Filesystem::Path &path);
        void saveToFile();

        NodeBase *addNode(std::unique_ptr<NodeBase> node);
        NodeBase *addNode(const std::string_view &name);
        void removeNode(NodeBase *node);

        const std::vector<std::unique_ptr<NodeBase>> &nodes() const;
        const NodeBase *node(uint32_t index) const;
        NodeBase *node(uint32_t index);

        TargetPin flowOutTarget(uint32_t source, uint32_t sourceIndex);
        TargetPin dataInSource(uint32_t target, uint32_t targetIndex);
        TargetPin dataOutTarget(uint32_t source, uint32_t sourceIndex);

        void connectFlow(uint32_t source, uint32_t sourceIndex, uint32_t target, uint32_t targetIndex);
        void connectDataIn(uint32_t target, uint32_t targetIndex, uint32_t source, uint32_t sourceIndex);
        void connectDataOut(uint32_t source, uint32_t sourceIndex, uint32_t target, uint32_t targetIndex);        
        
        void disconnectFlow(uint32_t index);
        void disconnectDataIn(uint32_t index);
        void disconnectDataOut(uint32_t index);

        std::vector<FlowOutPinPrototype> mFlowInPins;
        size_t mFlowOutPinCount = 0;
        std::vector<DataOutPinPrototype> mDataReceiverPins;
        std::vector<DataInPinPrototype> mDataProviderPins;
        std::vector<DataReceiverPinPrototype> mDataOutPins;
        std::vector<DataProviderPinPrototype> mDataInPins;

    protected:
        static const char *nodeCreationNames(size_t index);
        std::unique_ptr<NodeBase> createNode(const std::string_view &name);
        std::tuple<std::unique_ptr<NodeBase>> createNodeTuple(const std::string &name);
        std::tuple<std::string_view> storeNodeCreationData(const std::unique_ptr<NodeBase> &node) const;

    private:
        Filesystem::Path mPath;

        std::vector<std::unique_ptr<NodeBase>> mNodes;
    };

}
}
