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

    template <typename _Signature>
    struct algorithm {
        using Signature = _Signature;
    };
    template <typename _Signature>
    struct pred_sender {
        using Signature = _Signature;
    };
    struct succ_sender;

    using NodeResults = std::vector<ValueType>;

}
}
