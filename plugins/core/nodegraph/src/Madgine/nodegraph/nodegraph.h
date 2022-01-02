#pragma once

#include "Interfaces/filesystem/path.h"

#include "nodebase.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeGraph : Serialize::SerializableDataUnit {

        SERIALIZABLEUNIT(NodeGraph);

        NodeGraph();
        NodeGraph(const NodeGraph &other);
        NodeGraph(NodeGraph &&);
        ~NodeGraph();

        NodeGraph &operator=(const NodeGraph &other);

        std::string_view name() const;
        uint32_t generation() const;

        Serialize::StreamResult loadFromFile(const Filesystem::Path &path);
        void saveToFile();

        NodeBase *addNode(std::unique_ptr<NodeBase> node);
        NodeBase *addNode(std::string_view name);
        void removeNode(uint32_t index);

        const std::vector<std::unique_ptr<NodeBase>> &nodes() const;
        const NodeBase *node(IndexType<uint32_t,0> index) const;
        NodeBase *node(IndexType<uint32_t, 0> index);
        uint32_t nodeIndex(const NodeBase *node) const;

        Pin flowOutTarget(Pin source);
        Pin dataInSource(Pin target);
        Pin dataOutTarget(Pin source);

        ExtendedValueTypeDesc dataReceiverType(Pin source, bool bidir = true);
        ExtendedValueTypeDesc dataProviderType(Pin target, bool bidir = true);
        ExtendedValueTypeDesc dataInType(Pin source, bool bidir = true);
        ExtendedValueTypeDesc dataOutType(Pin target, bool bidir = true);

        uint32_t flowOutMask(Pin source, bool bidir = true);
        uint32_t flowInMask(Pin target, bool bidir = true);
        uint32_t dataReceiverMask(Pin source, bool bidir = true);
        uint32_t dataProviderMask(Pin target, bool bidir = true);
        uint32_t dataInMask(Pin source, bool bidir = true);
        uint32_t dataOutMask(Pin target, bool bidir = true);

        std::string_view flowOutName(Pin source);
        std::string_view flowInName(Pin target);
        std::string_view dataReceiverName(Pin source);
        std::string_view dataProviderName(Pin target);
        std::string_view dataInName(Pin source);
        std::string_view dataOutName(Pin target);        

        void connectFlow(Pin source, Pin target);
        void connectDataIn(Pin target, Pin source);
        void connectDataOut(Pin source, Pin target);

        void disconnectFlow(Pin source);
        void disconnectDataIn(Pin target);
        void disconnectDataOut(Pin source);

        bool checkDataProvider(uint32_t index);

        void onDataProviderRemove(const NodeBase *node, uint32_t index);

        std::vector<FlowInPinPrototype> mFlowInPins;
        std::vector<FlowOutPinPrototype> mFlowOutPins;
        std::vector<DataReceiverPinPrototype> mDataReceiverPins;
        std::vector<DataProviderPinPrototype> mDataProviderPins;
        std::vector<DataInPinPrototype> mDataInPins;
        std::vector<DataOutPinPrototype> mDataOutPins;        

    protected:
        std::unique_ptr<NodeBase> createNode(std::string_view name);
        Serialize::StreamResult readNode(Serialize::SerializeInStream &in, std::unique_ptr<NodeBase> &node);
        void writeNode(Serialize::SerializeOutStream &out, const std::unique_ptr<NodeBase> &node) const;

    private:
        Filesystem::Path mPath;

        std::vector<std::unique_ptr<NodeBase>> mNodes;

        uint32_t mGeneration = 0;
    };

}
}
