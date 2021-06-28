#pragma once

namespace Engine {
namespace NodeGraph {

    struct CodeGeneratorData {
        virtual ~CodeGeneratorData() = default;
    };

    struct MADGINE_NODEGRAPH_EXPORT CodeGenerator {

        CodeGenerator(const NodeGraph &graph);
        CodeGenerator(const CodeGenerator &) = delete;
        virtual ~CodeGenerator() = default;

        CodeGenerator &operator=(const CodeGenerator &) = delete;

        void generate(IndexType<uint32_t> &flowInOut, const NodeBase *startNode = nullptr);
        void generate(Pin &pin);

        virtual CodeGen::Statement read(uint32_t dataInIndex);
        virtual CodeGen::Statement write(uint32_t dataOutIndex, CodeGen::Statement statement);

        virtual CodeGen::File &file() = 0;

    protected:
        std::vector<std::unique_ptr<CodeGeneratorData>> mData;
        const NodeBase *mCurrentNode = nullptr;
        const NodeGraph &mGraph;    
        
    };

}
}