#pragma once

#include "pinprototypes.h"

#include "graphexecutable.h"


namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT NodePrototypeBase : Serialize::SerializableUnit<NodePrototypeBase, GraphExecutable> {

        SERIALIZABLEUNIT;

        virtual ~NodePrototypeBase() = default;

        virtual const std::string_view &name() const = 0;

        virtual size_t flowInCount() const = 0;
        virtual std::string_view flowInName(size_t index) const = 0;
        static uintptr_t flowInId(size_t index);

        size_t flowOutCount() const;
        virtual std::string_view flowOutName(size_t index) const = 0;
        TargetPin flowOutTarget(size_t index) const;
        static uintptr_t flowOutId(size_t index);

        size_t dataInCount() const;
        virtual std::string_view dataInName(size_t index) const = 0;
        virtual ExtendedValueTypeDesc dataInExpectedType(size_t index) const = 0;
        TargetPin dataInSource(size_t index) const;
        ValueType &dataInDefault(size_t index);
        static uintptr_t dataInId(size_t index);

        size_t dataOutCount() const;
        virtual std::string_view dataOutName(size_t index) const = 0;
        ExtendedValueTypeDesc dataOutType(size_t index) const;
        static uintptr_t dataOutId(size_t index);

        static PinDesc fromId(uintptr_t id);

        void connectFlow(size_t index, GraphExecutable *target, size_t targetIndex);
        void disconnectFlow(size_t index);
        void connectData(size_t index, GraphExecutable *source, size_t sourceIndex);
        void disconnectData(size_t index);

    protected:
        std::vector<FlowOutPinPrototype> mFlowOutPins;
        std::vector<DataInPinPrototype> mDataInPins;
        std::vector<DataOutPinPrototype> mDataOutPins;
    };

}
}

RegisterType(Engine::NodeGraph::NodePrototypeBase);