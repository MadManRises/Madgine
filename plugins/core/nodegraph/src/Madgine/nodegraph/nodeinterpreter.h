#pragma once

namespace Engine {
namespace NodeGraph {

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreter {

        NodeInterpreter(const NodeGraph &graph);
        NodeInterpreter(const NodeInterpreter &) = delete;

        NodeInterpreter &operator=(const NodeInterpreter &) = delete;

        uint32_t interpret(uint32_t flowIn = 0);

        ValueType read(uint32_t dataInIndex);
        void write(uint32_t dataOutIndex, const ValueType &v);

    private:
        const NodeGraph &mGraph;
        const NodeBase *mCurrentNode = nullptr;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;
    };

}
}