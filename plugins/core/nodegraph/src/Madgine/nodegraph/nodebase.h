#pragma once

#include "pins.h"

#include "Meta/serialize/virtualserializableunit.h"

#include "Meta/keyvalue/virtualscope.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodeBase : Serialize::VirtualSerializableUnitBase<VirtualScopeBase<>, Serialize::SerializableUnitBase> {

        SERIALIZABLEUNIT(NodeBase);

        NodeBase(NodeGraph &graph);
        virtual ~NodeBase() = default;

        virtual std::string_view name() const = 0;

        virtual size_t flowInCount() const { return 0; }
        virtual std::string_view flowInName(uint32_t index) const { throw 0; }
        static uint32_t flowInId(uint32_t index);

        virtual size_t flowOutCount() const { return 0; }
        virtual std::string_view flowOutName(uint32_t index) const { throw 0; }
        TargetPin flowOutTarget(uint32_t index) const;
        static uint32_t flowOutId(uint32_t index);

        virtual size_t dataInCount() const { return 0; }
        virtual std::string_view dataInName(uint32_t index) const { throw 0; }
        virtual ExtendedValueTypeDesc dataInType(uint32_t index) const { throw 0; }
        TargetPin dataInSource(uint32_t index) const;
        static uint32_t dataInId(uint32_t index);

        virtual size_t dataOutCount() const { return 0; }
        virtual std::string_view dataOutName(uint32_t index) const { throw 0; }
        virtual ExtendedValueTypeDesc dataOutType(uint32_t index) const { throw 0; }
        TargetPin dataOutTarget(uint32_t index) const;
        static uint32_t dataOutId(uint32_t index);

        virtual size_t dataReceiverCount() const { return 0; }
        virtual std::string_view dataReceiverName(uint32_t index) const { throw 0; }
        virtual ExtendedValueTypeDesc dataReceiverType(uint32_t index) const { throw 0; }
        static uint32_t dataReceiverId(uint32_t index);

        virtual size_t dataProviderCount() const { return 0; }
        virtual std::string_view dataProviderName(uint32_t index) const { throw 0; }
        virtual ExtendedValueTypeDesc dataProviderType(uint32_t index) const { throw 0; }
        static uint32_t dataProviderId(uint32_t index);

        static PinDesc pinFromId(uint32_t id);

        void connectFlow(uint32_t index, uint32_t target, uint32_t targetIndex);
        void connectDataIn(uint32_t index, uint32_t source, uint32_t sourceIndex);
        void connectDataOut(uint32_t index, uint32_t target, uint32_t targetIndex);

        void disconnectFlow(uint32_t index);
        void disconnectDataIn(uint32_t index);
        void disconnectDataOut(uint32_t index);

        virtual uint32_t interpret(NodeInterpreter &interpreter, uint32_t flowIn, std::unique_ptr<NodeInterpreterData> &data) const;
        virtual ValueType interpretRead(NodeInterpreter &interpreter, uint32_t providerIndex, std::unique_ptr<NodeInterpreterData> &data) const;
        virtual void interpretWrite(NodeInterpreter &interpreter, uint32_t receiverIndex, std::unique_ptr<NodeInterpreterData> &data, const ValueType &v) const;

    protected:
        void setup();

        NodeGraph &mGraph;

    private:
        std::vector<FlowOutPinPrototype> mFlowOutPins;
        std::vector<DataInPinPrototype> mDataInPins;
        std::vector<DataOutPinPrototype> mDataOutPins;
    };

}
}

RegisterType(Engine::NodeGraph::NodeBase);
