#pragma once

#include "pins.h"

#include "Meta/serialize/hierarchy/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

#include "nodeexecutionmask.h"

#include "Madgine/codegen/statement.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeBase : Serialize::VirtualSerializableDataBase<VirtualScopeBase<>, Serialize::SerializableDataUnit> {

        SERIALIZABLEUNIT(NodeBase)

        NodeBase(NodeGraph &graph);
        NodeBase(const NodeBase &other, NodeGraph &graph);
        virtual ~NodeBase() = default;

        virtual std::string_view name() const = 0;
        virtual std::string_view className() const = 0;

        virtual size_t flowInCount() const { return 0; }
        virtual std::string_view flowInName(uint32_t index) const { throw 0; }
        virtual uint32_t flowInMask(uint32_t index, bool bidir = true) const { return NodeExecutionMask::CPU; }
        const std::vector<Pin> &flowInSources(uint32_t index) const;
        static uint32_t flowInId(uint32_t index);

        virtual size_t flowOutCount() const { return 0; }
        virtual std::string_view flowOutName(uint32_t index) const { throw 0; }
        virtual uint32_t flowOutMask(uint32_t index, bool bidir = true) const { return NodeExecutionMask::CPU; }
        Pin flowOutTarget(uint32_t index) const;
        static uint32_t flowOutId(uint32_t index);

        virtual size_t dataInCount() const { return 0; }
        virtual std::string_view dataInName(uint32_t index) const { throw 0; }
        virtual uint32_t dataInMask(uint32_t index, bool bidir = true) const { return NodeExecutionMask::CPU; }
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const { throw 0; }
        virtual bool dataInVariadic() const { return false; }
        virtual ValueType dataInDefault(uint32_t index) const;
        Pin dataInSource(uint32_t index) const;
        static uint32_t dataInId(uint32_t index);

        virtual size_t dataOutCount() const { return 0; }
        virtual std::string_view dataOutName(uint32_t index) const { throw 0; }
        virtual uint32_t dataOutMask(uint32_t index, bool bidir = true) const { return NodeExecutionMask::CPU; }
        virtual ExtendedValueTypeDesc dataOutType(uint32_t index, bool bidir = true) const { throw 0; }
        virtual bool dataOutVariadic() const { return false; }
        Pin dataOutTarget(uint32_t index) const;
        static uint32_t dataOutId(uint32_t index);

        virtual size_t dataReceiverCount() const { return 0; }
        virtual std::string_view dataReceiverName(uint32_t index) const { throw 0; }
        virtual uint32_t dataReceiverMask(uint32_t index, bool bidir = true) const { return NodeExecutionMask::CPU; }
        virtual ExtendedValueTypeDesc dataReceiverType(uint32_t index, bool bidir = true) const { throw 0; }
        virtual bool dataReceiverVariadic() const { return false; }
        const std::vector<Pin> &dataReceiverSources(uint32_t index) const;
        static uint32_t dataReceiverId(uint32_t index);

        virtual size_t dataProviderCount() const { return 0; }
        virtual std::string_view dataProviderName(uint32_t index) const { throw 0; }
        virtual uint32_t dataProviderMask(uint32_t index, bool bidir = true) const { return NodeExecutionMask::CPU; }
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index, bool bidir = true) const { throw 0; }
        virtual bool dataProviderVariadic() const { return false; }
        const std::vector<Pin> &dataProviderTargets(uint32_t index) const;
        static uint32_t dataProviderId(uint32_t index);

        static PinDesc pinFromId(uint32_t id);

        virtual void onFlowOutUpdate(uint32_t index, Pin target, EdgeEvent event);
        virtual void onDataInUpdate(uint32_t index, Pin source, EdgeEvent event);
        virtual void onDataOutUpdate(uint32_t index, Pin target, EdgeEvent event);

        virtual void onFlowInUpdate(uint32_t index, Pin source, EdgeEvent event);
        virtual void onDataProviderUpdate(uint32_t index, Pin target, EdgeEvent event);
        virtual void onDataReceiverUpdate(uint32_t index, Pin source, EdgeEvent event);

        void removeFlowInPin(uint32_t index);
        void removeFlowOutPin(uint32_t index);
        void removeDataInPin(uint32_t index);
        void removeDataOutPin(uint32_t index);
        void removeDataReceiverPin(uint32_t index);
        void removeDataProviderPin(uint32_t index);

        void onFlowInRemove(Pin pin);
        void onFlowOutRemove(Pin pin);
        void onDataInRemove(Pin pin);
        void onDataOutRemove(Pin pin);
        void onDataReceiverRemove(Pin pin);
        void onDataProviderRemove(Pin pin);
        void onNodeReindex(uint32_t oldIndex, uint32_t newIndex);

        virtual void interpret(NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeInterpreterData> &data) const;
        virtual void interpretRead(NodeInterpreter &interpreter, ValueType &retVal, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const;
        virtual void interpretWrite(NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeInterpreterData> &data, const ValueType &v) const;

        virtual void generate(CodeGenerator &generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<CodeGeneratorData> &data) const;
        virtual CodeGen::Statement generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const;
        virtual CodeGen::Statement generateWrite(CodeGenerator &generator, uint32_t receiverIndex, std::unique_ptr<CodeGeneratorData> &data) const;

        virtual std::unique_ptr<NodeBase> clone(NodeGraph &graph) const = 0;

    protected:
        void setup();

        NodeGraph &mGraph;

        friend struct NodeGraph;

        std::vector<FlowOutPinPrototype> mFlowOutPins;
        std::vector<DataInPinPrototype> mDataInPins;
        std::vector<DataOutPinPrototype> mDataOutPins;

        std::vector<DataProviderPinPrototype> mDataProviderPins;
        std::vector<DataReceiverPinPrototype> mDataReceiverPins;
        std::vector<FlowInPinPrototype> mFlowInPins;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::NodeBase)
