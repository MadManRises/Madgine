#pragma once

namespace Engine {
namespace NodeGraph {

    struct NodeBase;
    struct NodeGraph;

    struct NodeReceiver;

    struct NodeInterpreter;
    struct NodeInterpreterData;

    struct CodeGenerator;
    struct CodeGeneratorData;

    struct Pin;

    struct FlowOutPinPrototype;
    struct DataInPinPrototype;
    struct DataOutPinPrototype;
    struct DataReceiverPinPrototype;
    struct DataProviderPinPrototype;

    enum EdgeEvent {
        CONNECT,
        DISCONNECT,
        UPDATE
    };

    template <typename Signature>
    struct algorithm;

    using NodeResults = std::vector<ValueType>;

}
}
