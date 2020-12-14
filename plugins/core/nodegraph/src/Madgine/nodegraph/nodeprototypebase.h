#pragma once

#include "pinprototypes.h"

#include "graphexecutable.h"


namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodePrototypeBase : GraphExecutable {

        SERIALIZABLEUNIT(NodePrototypeBase);

        virtual ~NodePrototypeBase() = default;

        virtual const std::string_view &name() const = 0;

        virtual size_t flowInCount() const = 0;
        virtual std::string_view flowInName(uint32_t index) const = 0;
        static uint32_t flowInId(uint32_t index);

        size_t flowOutCount() const;
        virtual std::string_view flowOutName(uint32_t index) const = 0;
        TargetPin flowOutTarget(uint32_t index) const;
        static uint32_t flowOutId(uint32_t index);

        size_t dataInCount() const;
        virtual std::string_view dataInName(uint32_t index) const = 0;
        virtual ExtendedValueTypeDesc dataInExpectedType(uint32_t index) const = 0;
        TargetPin dataInSource(uint32_t index) const;
        ValueType &dataInDefault(uint32_t index);
        static uint32_t dataInId(uint32_t index);

        size_t dataOutCount() const;
        virtual std::string_view dataOutName(uint32_t index) const = 0;
        ExtendedValueTypeDesc dataOutType(uint32_t index) const;
        static uint32_t dataOutId(uint32_t index);

        static PinDesc fromId(uint32_t id);

        void connectFlow(uint32_t index, GraphExecutable *target, uint32_t targetIndex);
        void disconnectFlow(uint32_t index);
        void connectData(uint32_t index, GraphExecutable *source, uint32_t sourceIndex);
        void disconnectData(uint32_t index);

    protected:
        std::vector<FlowOutPinPrototype> mFlowOutPins;
        std::vector<DataInPinPrototype> mDataInPins;
        std::vector<DataOutPinPrototype> mDataOutPins;
    };

}
}

RegisterType(Engine::NodeGraph::NodePrototypeBase);
