#pragma once

namespace Engine {
namespace NodeGraph {

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreter {

        NodeInterpreter() = default;
        NodeInterpreter(const NodeGraph *graph);
        NodeInterpreter(const NodeInterpreter &) = delete;
        NodeInterpreter(NodeInterpreter &&) = default;
        virtual ~NodeInterpreter() = default;

        NodeInterpreter &operator=(const NodeInterpreter &) = delete;

        void interpret(IndexType<uint32_t> &flowIn, const ArgumentList &args, const NodeBase *startNode = nullptr);
        void interpret(Pin &pin, const ArgumentList &args);

        void branch(IndexType<uint32_t> flow);
        void branch(Pin &pin);

        void read(ValueType &retVal, uint32_t dataInIndex);
        void write(uint32_t dataOutIndex, const ValueType &v);

        void setGraph(const NodeGraph *graph);
        const NodeGraph *graph() const;

        std::unique_ptr<NodeInterpreterData> &data(uint32_t index);

    private:
        const NodeGraph *mGraph = nullptr;
        const NodeBase *mCurrentNode = nullptr;

        const ArgumentList *mCurrentArguments = nullptr;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;
        IndexType<uint32_t> mGraphGeneration;
    };

}
}